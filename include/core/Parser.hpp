#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "ExpenseStruct.hpp"

enum MyEnum
{
	RAKUTEN,
	MITSUBISHI,
	RAKUTEN_CREDIT
};

class Parser
{
  private:
	Parser()                          = delete;
	~Parser()                         = delete;
	Parser(const Parser &)            = delete;
	Parser &operator=(const Parser &) = delete;

  public:
	static std::vector<std::string>    splitIntoLines(std::string data);
	static std::vector<Expense>        parseExpenses(std::string data, std::chrono::year year);
	static std::string                 convertFullWidthToAscii(const std::string &input);
	static std::vector<std::string>    removeEmptyLines(const std::vector<std::string> &lines);
	static int                         whichBank(std::string line);
	static std::vector<Expense>        fillExpensesStructMitsubishi(std::vector<std::string>, std::chrono::year year);
	static std::vector<Expense>        fillExpensesStructRakutenCredit(std::vector<std::string>);
	static std::vector<Expense>        fillExpensesStructRakuten(std::vector<std::string>);
	static std::string                 removeIsolatedNumber(const std::string &input);
	static std::chrono::year_month_day parseShortDate(std::chrono::year year, const std::string &shortDate);
};
