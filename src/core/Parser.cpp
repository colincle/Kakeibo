#include "Parser.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <stdexcept>

std::vector<Expense>	Parser::parseExpenses(std::string data, std::chrono::year year)
{
	data = convertFullWidthToAscii(data);
	std::vector<std::string> lines = splitIntoLines(data);
	return fillExpensesStruct(lines, year);
}

std::string Parser::convertFullWidthToAscii(const std::string& input)
{
	std::string output;

	for(size_t i = 0; i < input.size();)
	{
		unsigned char c1 = static_cast<unsigned char>(input[i]);
		unsigned char c2 = i + 1 < input.size() ? static_cast<unsigned char>(input[i + 1]) : 0;
		unsigned char c3 = i + 2 < input.size() ? static_cast<unsigned char>(input[i + 2]) : 0;

		// Full-width 0-9: EF BC 90 - 99
		if(c1 == 0xEF && c2 == 0xBC && c3 >= 0x90 && c3 <= 0x99)
		{
			output += '0' + (c3 - 0x90);
			i += 3;
		}
		// Full-width A-Z: EF BC A1-AA
		else
			if(c1 == 0xEF && c2 == 0xBC && c3 >= 0xA1 && c3 <= 0xBA)
			{
				output += 'A' + (c3 - 0xA1);
				i += 3;
			}
			// Full-width a-z: EF BD 81-9A
			else
				if(c1 == 0xEF && c2 == 0xBD && c3 >= 0x81 && c3 <= 0x9A)
				{
					output += 'a' + (c3 - 0x81);
					i += 3;
				}
				// Full-width space: E3 80 80
				else
					if(c1 == 0xE3 && c2 == 0x80 && c3 == 0x80)
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

std::vector<std::string>	Parser::splitIntoLines(std::string data)
{
	std::vector<std::string> lines;
	std::istringstream stream(data);
	std::string line;

	while(std::getline(stream, line))
		lines.push_back(line);

	return lines;
}

std::vector<Expense> Parser::fillExpensesStruct(std::vector<std::string> lines, std::chrono::year year)
{
	std::vector<Expense> expenses;

	for(const std::string& line : lines)
	{
		std::istringstream stream(line);
		std::string date, debitStr, creditStr, info;

		std::getline(stream, date, '\t');
		std::getline(stream, debitStr, '\t');
		std::getline(stream, creditStr, '\t');
		std::getline(stream, info, '\t');

		if(date.empty() || (debitStr.empty() && creditStr.empty()) || info.empty())
			continue;

		Expense e;
		e.date = parseShortDate(year, date);
		e.debit = std::stoi(debitStr);
		e.credit = std::stoi(creditStr);
		e.info = removeIsolatedNumber(info);;
		expenses.push_back(e);
	}

	return expenses;
}

std::string Parser::removeIsolatedNumber(const std::string& input)
{
	std::string output;
	std::istringstream stream(input);
	std::string word;
	bool first = true;

	while(stream >> word)
	{
		bool isNumber = !word.empty() && std::all_of(word.begin(), word.end(), ::isdigit);

		if(isNumber)
			continue;

		if(!first)
			output += ' ';

		output += word;
		first = false;
	}

	return output;
}

std::chrono::year_month_day Parser::parseShortDate(std::chrono::year year, const std::string& shortDate)
{
	std::istringstream stream(shortDate);
	int month, day;
	char slash;

	stream >> month >> slash >> day;

	if(slash != '/' || month < 1 || month > 12 || day < 1 || day > 31)
		throw std::invalid_argument("Invalid date format: " + shortDate);

	return std::chrono::year_month_day{year, std::chrono::month{static_cast<unsigned>(month)}, std::chrono::day{static_cast<unsigned>(day)}};
}
