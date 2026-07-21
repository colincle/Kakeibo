// Minimal dependency-free tests for the bank-statement parser.
// Returns the number of failed checks (0 == success), which CTest treats as
// pass/fail. Kept framework-free so it links against Parser.cpp alone.

#include "Parser.hpp"

#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

static int g_failures = 0;

#define CHECK(cond)                                                     \
	do                                                                  \
	{                                                                   \
		if ( !(cond) )                                                  \
		{                                                               \
			std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
			++g_failures;                                               \
		}                                                               \
	} while ( 0 )

static bool dateIs(const Expense &e, int y, unsigned m, unsigned d)
{
	return int(e.date.year()) == y && unsigned(e.date.month()) == m && unsigned(e.date.day()) == d;
}

static void test_whichBank()
{
	CHECK(Parser::whichBank("2024/01/15\tAmazon\tx\ty\t3000") == Bank::RakutenCredit); // 2 slashes
	CHECK(Parser::whichBank("01/15\t1500\t\tSHOP") == Bank::Mitsubishi);               // 1 slash
	CHECK(Parser::whichBank("2024/01/15") == Bank::Rakuten);                           // no tab
}

static void test_convertFullWidthToAscii()
{
	CHECK(Parser::convertFullWidthToAscii("ＡＢＣ１２３") == "ABC123");
	CHECK(Parser::convertFullWidthToAscii("ｈｅｌｌｏ") == "hello");
	CHECK(Parser::convertFullWidthToAscii("a　b") == "a b"); // full-width space -> ascii space
	CHECK(Parser::convertFullWidthToAscii("plain") == "plain");
}

static void test_removeIsolatedNumber()
{
	CHECK(Parser::removeIsolatedNumber("SEVEN 999 ELEVEN") == "SEVEN ELEVEN");
	CHECK(Parser::removeIsolatedNumber("12345 SHOP") == "SHOP");
	CHECK(Parser::removeIsolatedNumber("NO NUMBERS") == "NO NUMBERS");
}

static void test_parseShortDate()
{
	auto d = Parser::parseShortDate(std::chrono::year {2024}, "3/15");
	CHECK(int(d.year()) == 2024 && unsigned(d.month()) == 3 && unsigned(d.day()) == 15);

	bool threw = false;

	try
	{
		Parser::parseShortDate(std::chrono::year {2024}, "13/40");
	}
	catch ( const std::exception & )
	{
		threw = true;
	}

	CHECK(threw);
}

static void test_parse_rakuten()
{
	// Rakuten bank export: 3 lines per record, no tabs.
	std::string data = "2024/01/15\nFamilyMart\n-500\n";
	auto        exp  = Parser::parseExpenses(data, std::chrono::year {2024});

	CHECK(exp.size() == 1);

	if ( exp.size() == 1 )
	{
		CHECK(exp[0].amount == -500);
		CHECK(exp[0].info == "FamilyMart");
		CHECK(exp[0].isCredit == false);
		CHECK(dateIs(exp[0], 2024, 1, 15));
	}
}

static void test_parse_rakuten_credit()
{
	// Tab separated, first column "yyyy/mm/dd" (2 slashes).
	std::string data = "2024/02/20\tAmazon\tx\ty\t3000\n";
	auto        exp  = Parser::parseExpenses(data, std::chrono::year {2024});

	CHECK(exp.size() == 1);

	if ( exp.size() == 1 )
	{
		CHECK(exp[0].amount == -3000); // credit charges stored as negative
		CHECK(exp[0].info == "Amazon");
		CHECK(exp[0].isCredit == true);
		CHECK(dateIs(exp[0], 2024, 2, 20));
	}
}

static void test_parse_rakuten_credit_new_format()
{
	// Newer export splits each record over two lines: "date \t description",
	// then "person \t installments \t amount \t ..." with blank lines between.
	std::string data =
	    "2026/07/08\tVISA国内利用 VS ロ-ソン\n"
	    "\n"
	    "本人\t1回払い\t¥ 1,185\t支払金額を変更する\n"
	    "\n\n\n"
	    "2026/07/08\tVISA国内利用 VS ロ-ソン\n"
	    "\n"
	    "家族\t1回払い\t¥ 1,078\t支払金額を変更する\n";
	auto exp = Parser::parseExpenses(data, std::chrono::year {2026});

	CHECK(exp.size() == 2);

	if ( exp.size() == 2 )
	{
		CHECK(exp[0].amount == -1185);
		CHECK(exp[0].info == "VISA国内利用 VS ロ-ソン");
		CHECK(exp[0].isCredit == true);
		CHECK(dateIs(exp[0], 2026, 7, 8));

		CHECK(exp[1].amount == -1078);
		CHECK(dateIs(exp[1], 2026, 7, 8));
	}
}

static void test_parse_mitsubishi()
{
	// Tab separated, first column "mm/dd" (1 slash); year comes from the arg.
	std::string data = "01/10\t1500\t\tSEVEN 999\n";
	auto        exp  = Parser::parseExpenses(data, std::chrono::year {2024});

	CHECK(exp.size() == 1);

	if ( exp.size() == 1 )
	{
		CHECK(exp[0].amount == -1500); // debit -> negative
		CHECK(exp[0].info == "SEVEN"); // isolated number stripped
		CHECK(dateIs(exp[0], 2024, 1, 10));
	}
}

static void test_parse_empty()
{
	CHECK(Parser::parseExpenses("", std::chrono::year {2024}).empty());
	CHECK(Parser::parseExpenses("\n\n\n", std::chrono::year {2024}).empty());
}

static Expense makeExpense(int y, unsigned m, unsigned d, int amount, const std::string &info, bool isCredit = false)
{
	Expense e;
	e.date     = std::chrono::year {y} / std::chrono::month {m} / std::chrono::day {d};
	e.amount   = amount;
	e.info     = info;
	e.envelope = "";
	e.isCredit = isCredit;
	return e;
}

static void test_filter_new_expenses()
{
	Expense a = makeExpense(2024, 1, 15, -500, "FamilyMart");
	Expense b = makeExpense(2024, 1, 16, -1200, "SEVEN");
	Expense c = makeExpense(2024, 1, 17, -300, "Drugstore");

	// Nothing stored yet: every pasted row is new.
	{
		auto out = Parser::filterNewExpenses({a, b, c}, {});
		CHECK(out.size() == 3);
	}

	// Everything already stored: re-pasting the same list adds nothing, whatever
	// the order.
	{
		auto out = Parser::filterNewExpenses({c, a, b}, {a, b, c});
		CHECK(out.empty());
	}

	// Mixed paste: only the rows missing from storage survive, and out-of-order
	// pastes are handled.
	{
		auto out = Parser::filterNewExpenses({c, a, b}, {a});
		CHECK(out.size() == 2);

		if ( out.size() == 2 )
		{
			CHECK(out[0].info == "Drugstore"); // c, kept in paste order
			CHECK(out[1].info == "SEVEN");     // b
		}
	}

	// Two genuinely distinct expenses sharing date, amount and shop: both are
	// kept when neither is stored yet.
	{
		auto out = Parser::filterNewExpenses({a, a}, {});
		CHECK(out.size() == 2);
	}

	// One such duplicate is already stored: re-pasting both keeps exactly one.
	{
		auto out = Parser::filterNewExpenses({a, a}, {a});
		CHECK(out.size() == 1);
	}

	// Storage holds two copies, paste holds three: only the extra one is new.
	{
		auto out = Parser::filterNewExpenses({a, a, a}, {a, a});
		CHECK(out.size() == 1);
	}

	// Same date, amount and shop but different credit flag are treated as
	// distinct records (a bank debit vs a credit charge).
	{
		Expense bank   = makeExpense(2024, 1, 15, -500, "FamilyMart", false);
		Expense credit = makeExpense(2024, 1, 15, -500, "FamilyMart", true);
		auto    out    = Parser::filterNewExpenses({bank}, {credit});
		CHECK(out.size() == 1);
	}
}

int main()
{
	test_whichBank();
	test_convertFullWidthToAscii();
	test_removeIsolatedNumber();
	test_parseShortDate();
	test_parse_rakuten();
	test_parse_rakuten_credit();
	test_parse_rakuten_credit_new_format();
	test_parse_mitsubishi();
	test_parse_empty();
	test_filter_new_expenses();

	if ( g_failures == 0 )
		std::printf("All parser tests passed.\n");
	else
		std::printf("%d check(s) failed.\n", g_failures);

	return g_failures;
}
