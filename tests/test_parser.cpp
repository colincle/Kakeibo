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

int main()
{
	test_whichBank();
	test_convertFullWidthToAscii();
	test_removeIsolatedNumber();
	test_parseShortDate();
	test_parse_rakuten();
	test_parse_rakuten_credit();
	test_parse_mitsubishi();
	test_parse_empty();

	if ( g_failures == 0 )
		std::printf("All parser tests passed.\n");
	else
		std::printf("%d check(s) failed.\n", g_failures);

	return g_failures;
}
