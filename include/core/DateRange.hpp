#pragma once

#include <chrono>
#include <optional>
#include <utility>

// Earliest and latest year_month across the expenses of every envelope, or
// nullopt when there are no expenses at all. Shared by the History and Stats
// pages, which both need the available date span.
std::optional<std::pair<std::chrono::year_month, std::chrono::year_month>> expenseDateRange();
