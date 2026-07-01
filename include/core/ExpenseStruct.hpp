#pragma once

#include "json.hpp"

#include <chrono>
#include <sstream>
#include <string>

using json = nlohmann::json;

struct Expense
{
	std::chrono::year_month_day date;
	int                         amount;
	std::string                 info;
	std::string                 envelope;
	bool                        isCredit;
};

inline void to_json(json &j, const Expense &e)
{
	auto twoDigit = [](unsigned val)
	{
		std::string s = std::to_string(val);

		if ( s.length() < 2 )
			s.insert(0, "0");

		return s;
	};

	std::string dateStr = std::to_string(int(e.date.year())) + "-" + twoDigit(unsigned(e.date.month())) + "-" + twoDigit(unsigned(e.date.day()));

	j = json {{"date", dateStr},
	          {"amount", e.amount},
	          {"info", e.info},
	          {"enveloppe", e.envelope},
	          {"isCredit", e.isCredit}};
}

inline void from_json(const json &j, Expense &e)
{
	std::string dateStr = j.at("date").get<std::string>();
	int         y, m, d;
	char        sep1, sep2;
	std::istringstream(dateStr) >> y >> sep1 >> m >> sep2 >> d;
	e.date = std::chrono::year(y) / std::chrono::month(static_cast<unsigned>(m)) / std::chrono::day(static_cast<unsigned>(d));

	e.amount   = j.at("amount").get<int>();
	e.info     = j.at("info").get<std::string>();
	e.envelope = j.at("enveloppe").get<std::string>();
	e.isCredit = j.contains("isCredit") ? j.at("isCredit").get<bool>() : false;
}
