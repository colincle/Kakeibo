#pragma once //WIP

#include "Parser"

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
	static void addExpense(QWidget* parent, Expense e);
};
