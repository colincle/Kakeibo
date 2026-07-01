#include "DateRange.hpp"
#include "Globals.hpp"

std::optional<std::pair<std::chrono::year_month, std::chrono::year_month>> expenseDateRange()
{
	std::optional<std::chrono::sys_days> minDate, maxDate;

	for ( const auto &env : g_envelopeManager.getEnvelopes() )
	{
		for ( const auto &exp : env.getExpenses() )
		{
			if ( !minDate || exp.date < *minDate )
				minDate = exp.date;

			if ( !maxDate || exp.date > *maxDate )
				maxDate = exp.date;
		}
	}

	if ( !minDate || !maxDate )
		return std::nullopt;

	auto ymdMin = std::chrono::year_month_day {*minDate};
	auto ymdMax = std::chrono::year_month_day {*maxDate};

	return std::make_pair(
	    std::chrono::year_month {ymdMin.year(), ymdMin.month()},
	    std::chrono::year_month {ymdMax.year(), ymdMax.month()});
}
