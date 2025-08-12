#include "Parser.hpp"
#include "ExpenseStruct.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<Expense> Parser::parseExpenses(std::string data, std::chrono::year year)
{
	data                           = convertFullWidthToAscii(data);
	std::vector<std::string> lines = splitIntoLines(data);
	lines                          = removeEmptyLines(lines);

	switch ( whichBank(lines[0]) )
	{
	case RAKUTEN:
		return fillExpensesStructRakuten(lines);

	case MITSUBISHI:
		return fillExpensesStructMitsubishi(lines, year);

	case RAKUTEN_CREDIT:
		return fillExpensesStructRakutenCredit(lines);

	default:
		return {};
	}
}

std::string Parser::convertFullWidthToAscii(const std::string &input)
{
	std::string output;

	for ( size_t i = 0; i < input.size(); )
	{
		unsigned char c1 = static_cast<unsigned char>(input[i]);
		unsigned char c2 = i + 1 < input.size() ? static_cast<unsigned char>(input[i + 1]) : 0;
		unsigned char c3 = i + 2 < input.size() ? static_cast<unsigned char>(input[i + 2]) : 0;

		// Full-width 0-9: EF BC 90 - 99
		if ( c1 == 0xEF && c2 == 0xBC && c3 >= 0x90 && c3 <= 0x99 )
		{
			output += '0' + (c3 - 0x90);
			i += 3;
		}
		// Full-width A-Z: EF BC A1-AA
		else if ( c1 == 0xEF && c2 == 0xBC && c3 >= 0xA1 && c3 <= 0xBA )
		{
			output += 'A' + (c3 - 0xA1);
			i += 3;
		}
		// Full-width a-z: EF BD 81-9A
		else if ( c1 == 0xEF && c2 == 0xBD && c3 >= 0x81 && c3 <= 0x9A )
		{
			output += 'a' + (c3 - 0x81);
			i += 3;
		}
		// Full-width space: E3 80 80
		else if ( c1 == 0xE3 && c2 == 0x80 && c3 == 0x80 )
		{
			output += ' ';
			i += 3;
		}
		else
		{
			output += input[i];
			++i;
		}
	}

	return output;
}

std::vector<std::string> Parser::splitIntoLines(std::string data)
{
	std::vector<std::string> lines;
	std::istringstream       stream(data);
	std::string              line;

	while ( std::getline(stream, line) )
		lines.push_back(line);

	return lines;
}

std::vector<std::string> Parser::removeEmptyLines(const std::vector<std::string> &lines)
{
	std::vector<std::string> result;
	result.reserve(lines.size());

	for ( const auto &line : lines )
	{
		if ( !line.empty() )
			result.push_back(line);
	}

	return result;
}

int Parser::whichBank(std::string line)
{
	size_t pos = line.find('\t');

	if ( pos == std::string::npos )
		return 0;

	line = line.substr(0, pos);
	return static_cast<int>(std::count(line.begin(), line.end(), '/'));
}

std::vector<Expense> Parser::fillExpensesStructRakutenCredit(std::vector<std::string> lines)
{
	std::vector<Expense> expenses;

	for ( const std::string &line : lines )
	{
		std::istringstream stream(line);
		std::string        dateStr, description, tmp, amountStr;

		std::getline(stream, dateStr, '\t');
		std::getline(stream, description, '\t');
		std::getline(stream, tmp, '\t');
		std::getline(stream, tmp, '\t');
		std::getline(stream, amountStr, '\t');

		if ( dateStr.empty() || amountStr.empty() )
			continue;

		auto trim = [](std::string &s)
		{
			s.erase(0, s.find_first_not_of(" \t\r\n"));
			s.erase(s.find_last_not_of(" \t\r\n") + 1);
		};
		trim(amountStr);

		amountStr.erase(std::remove_if(amountStr.begin(), amountStr.end(),
		                               [](unsigned char c)
		                               { return !std::isdigit(c); }),
		                amountStr.end());

		Expense e;
		{
			int  y, m, d;
			char sep1, sep2;
			std::istringstream(dateStr) >> y >> sep1 >> m >> sep2 >> d;
			e.date = std::chrono::year(y) / std::chrono::month(static_cast<unsigned>(m)) / std::chrono::day(static_cast<unsigned>(d));
		}

		int amount  = std::stoi(amountStr);
		e.amount    = -amount;
		e.info      = description;
		e.enveloppe = "";
		e.isCredit  = true;

		expenses.push_back(e);
	}

	return expenses;
}

std::vector<Expense> Parser::fillExpensesStructRakuten(std::vector<std::string> lines)
{
	std::vector<Expense> expenses;

	auto isDate = [](const std::string &s)
	{
		int                y, m, d;
		char               a, b;
		std::istringstream ss(s);
		return (ss >> y >> a >> m >> b >> d) && a == '/' && b == '/';
	};
	auto trim = [](std::string &s)
	{
		s.erase(0, s.find_first_not_of(" \t\r\n"));
		s.erase(s.find_last_not_of(" \t\r\n") + 1);
	};

	size_t i = 0;

	while ( i + 2 < lines.size() )
	{
		if ( !isDate(lines[i]) )
		{
			++i;
			continue;
		}

		std::string dateStr     = lines[i];
		std::string description = lines[i + 1];
		std::string amountStr   = lines[i + 2];

		trim(amountStr);
		amountStr.erase(std::remove_if(amountStr.begin(), amountStr.end(),
		                               [](unsigned char c)
		                               { return !(std::isdigit(c) || c == '-'); }),
		                amountStr.end());

		if ( amountStr.empty() || amountStr == "-" )
		{
			i += (i + 3 < lines.size() && !isDate(lines[i + 3])) ? 4 : 3;
			continue;
		}

		Expense e;
		int     y, m, d;
		char    sep1, sep2;
		std::istringstream(dateStr) >> y >> sep1 >> m >> sep2 >> d;
		e.date = std::chrono::year(y) / std::chrono::month(static_cast<unsigned>(m)) / std::chrono::day(static_cast<unsigned>(d));

		e.amount    = std::stoi(amountStr);
		e.info      = description;
		e.enveloppe = "";
		e.isCredit  = (e.amount > 0);

		expenses.push_back(e);

		i += (i + 3 < lines.size() && !isDate(lines[i + 3])) ? 4 : 3;
	}

	return expenses;
}

std::vector<Expense> Parser::fillExpensesStructMitsubishi(std::vector<std::string> lines, std::chrono::year year)
{
	std::vector<Expense> expenses;

	for ( const std::string &line : lines )
	{
		std::istringstream stream(line);
		std::string        date, debitStr, creditStr, info;

		std::getline(stream, date, '\t');
		std::getline(stream, debitStr, '\t');
		std::getline(stream, creditStr, '\t');
		std::getline(stream, info, '\t');

		if ( date.empty() || (debitStr.empty() && creditStr.empty()) || info.empty() )
			continue;

		auto trim = [](std::string &s)
		{
			s.erase(0, s.find_first_not_of(" \t\r\n"));
			s.erase(s.find_last_not_of(" \t\r\n") + 1);
			s.erase(std::remove(s.begin(), s.end(), ','), s.end());
			s.erase(std::remove(s.begin(), s.end(), '.'), s.end());
		};

		trim(debitStr);
		trim(creditStr);

		Expense e;
		e.date = parseShortDate(year, date);

		if ( !debitStr.empty() )
			e.amount = -std::stoi(debitStr);
		else if ( !creditStr.empty() )
			e.amount = std::stoi(creditStr);
		else
			continue;

		e.info     = removeIsolatedNumber(info);
		e.isCredit = false;
		expenses.push_back(e);
	}

	return expenses;
}

std::string Parser::removeIsolatedNumber(const std::string &input)
{
	std::string        output;
	std::istringstream stream(input);
	std::string        word;
	bool               first = true;

	while ( stream >> word )
	{
		bool isNumber = !word.empty() && std::all_of(word.begin(), word.end(), ::isdigit);

		if ( isNumber )
			continue;

		if ( !first )
			output += ' ';

		output += word;
		first = false;
	}

	return output;
}

std::chrono::year_month_day Parser::parseShortDate(std::chrono::year year, const std::string &shortDate)
{
	std::istringstream stream(shortDate);
	int                month, day;
	char               slash;

	stream >> month >> slash >> day;

	if ( slash != '/' || month < 1 || month > 12 || day < 1 || day > 31 )
		throw std::invalid_argument("Invalid date format: " + shortDate);

	return std::chrono::year_month_day {year, std::chrono::month {static_cast<unsigned>(month)}, std::chrono::day {static_cast<unsigned>(day)}};
}
