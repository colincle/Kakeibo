#include "ImportExpenses.hpp" //WIP
#include "Globals.hpp"
#include "Parser.hpp"
#include <QWidget>
#include <QPlainTextEdit>
#include <chrono>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDate>
#include <QDialog>

void ImportExpenses::import(QWidget* parent)
{
	auto [yearInt, data] = showImportDialog(parent);
	if (yearInt < 0 || data.empty())
		return;

	std::chrono::year year{yearInt};
	std::vector<Expense> expenses = Parser::parseExpenses(data, year);
	for (const Expense& e : expenses)
		addExpense(parent,  e);
}

static std::pair<int, std::string> showImportDialog(QWidget* parent)
{
	QDialog dialog(parent);
	dialog.setWindowTitle("Import Expenses");

	QVBoxLayout* layout = new QVBoxLayout(&dialog);

	QLabel* yearLabel = new QLabel("Year:");
	yearLabel->setStyleSheet("color: black;");
	layout->addWidget(yearLabel);

	QComboBox* yearCombo = new QComboBox();
	int currentYear = QDate::currentDate().year();
	for (int y = 1900; y <= 2100; ++y)
		yearCombo->addItem(QString::number(y));
	yearCombo->setCurrentText(QString::number(currentYear));
	yearCombo->setStyleSheet("color: black;");
	layout->addWidget(yearCombo);

	QLabel* inputLabel = new QLabel("Data:");
	inputLabel->setStyleSheet("color: black;");
	layout->addWidget(inputLabel);

	QPlainTextEdit* inputField = new QPlainTextEdit();
	inputField->setPlaceholderText("Paste expense data...");
	inputField->setStyleSheet("color: black;");
	inputField->setMinimumHeight(300);
	layout->addWidget(inputField);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	for (auto* button : buttons->findChildren<QPushButton*>())
		button->setStyleSheet("color: black;");
	layout->addWidget(buttons);

	QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	if (dialog.exec() != QDialog::Accepted)
		return { -1, "" };

	int year = yearCombo->currentText().toInt();
	std::string text = inputField->toPlainText().toStdString();
	return { year, text };
}

void addExpense(QWidget* parent, const Expense& e)
{
	const auto& enveloppes = g_enveloppeManager.getEnveloppes();
	bool matched = false;

	for (const auto& env : enveloppes)
	{
		for (const std::string& type : env.getTypes())
		{
			if (e.info == type)
			{
				// full match: do something
				matched = true;
				break;
			}
		}

		if (matched)
			break;
	}
	if (!matched)
	{
		// no match found: do something else
	}
}
