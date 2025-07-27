#pragma once

#include "ExpenseStruct.hpp"
#include "Enveloppe.hpp"

#include <string>
#include <vector>
#include <utility>

#include <QString>

class QWidget;
class QComboBox;
class QVBoxLayout;
class QPlainTextEdit;
class QDialog;
class QDialogButtonBox;

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
	static QString setDialogStyleSheet();
	static QComboBox* createYearSelector(QWidget* parent, QVBoxLayout* layout);
	static QPlainTextEdit* createTextInput(QWidget* parent, QVBoxLayout* layout);
	static void addDialogButtons(QDialog* dialog, QVBoxLayout* layout);

	static std::string selectEnveloppeDialog(QWidget* parent, const std::vector<Enveloppe>& enveloppes, const Expense& e);
	static QString buildDialogLabel(const Expense& e);
	static QDialogButtonBox* createDialogButtons(QDialog* parent);
};
