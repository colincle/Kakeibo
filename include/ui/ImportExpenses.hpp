#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Parser.hpp"
#include "ExpenseStruct.hpp"
#include "Enveloppe.hpp"

class QWidget;

class ImportExpenses final
{
private:
	ImportExpenses() = delete;
	~ImportExpenses() = delete;
	ImportExpenses(const ImportExpenses&) = delete;
	ImportExpenses& operator=(const ImportExpenses&) = delete;

public:

	static void import(QWidget* parent);
	static void addExpense(QWidget* parent, const Expense& e);
	static std::pair<int, std::string> showImportDialog(QWidget* parent);
	static std::string selectEnveloppeDialog(QWidget* parent, const std::vector<Enveloppe>& enveloppes, const Expense& e);
};
