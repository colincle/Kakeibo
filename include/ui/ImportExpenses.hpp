#pragma once

#include "Envelope.hpp"
#include "ExpenseStruct.hpp"

#include <string>
#include <utility>
#include <vector>

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
	ImportExpenses()                                  = delete;
	~ImportExpenses()                                 = delete;
	ImportExpenses(const ImportExpenses &)            = delete;
	ImportExpenses &operator=(const ImportExpenses &) = delete;

  public:
	static void import(QWidget *parent);

	static void addExpense(QWidget *parent, const Expense &e);

	static std::pair<int, std::string> showImportDialog(QWidget *parent);
	static QString                     setDialogStyleSheet();
	static QComboBox                  *createYearSelector(QWidget *parent, QVBoxLayout *layout);
	static QPlainTextEdit             *createTextInput(QWidget *parent, QVBoxLayout *layout);
	static void                        addDialogButtons(QDialog *dialog, QVBoxLayout *layout);

	static std::string       selectEnvelopeDialog(QWidget *parent, const std::vector<Envelope> &envelopes, const Expense &e, bool &rememberType);
	static QString           buildDialogLabel(const Expense &e);
	static QDialogButtonBox *createDialogButtons(QDialog *parent);
};
