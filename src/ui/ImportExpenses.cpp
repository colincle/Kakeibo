#include "ImportExpenses.hpp"
#include "Globals.hpp"
#include "Parser.hpp"
#include "Enveloppe.hpp"

#include <QWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDate>
#include <QDialog>
#include <QInputDialog>

#include <chrono>
#include <vector>

void ImportExpenses::import(QWidget* parent)
{
	auto [yearInt, data] = showImportDialog(parent);

	if(yearInt < 0 || data.empty())
		return;

	std::chrono::year year{yearInt};
	std::vector<Expense> expenses = Parser::parseExpenses(data, year);

	for(const Expense& e : expenses)
		addExpense(parent, e);
}

std::pair<int, std::string> ImportExpenses::showImportDialog(QWidget* parent)
{
	QDialog dialog(parent);
	dialog.setWindowTitle("Import Expenses");

	QVBoxLayout* layout = new QVBoxLayout(&dialog);

	QLabel* yearLabel = new QLabel("Year:");
	yearLabel->setStyleSheet("color: black;");
	layout->addWidget(yearLabel);

	QComboBox* yearCombo = new QComboBox();
	int currentYear = QDate::currentDate().year();

	for(int y = 1900; y <= 2100; ++y)
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

	for(auto * button : buttons->findChildren<QPushButton * >())
		button->setStyleSheet("color: black;");

	layout->addWidget(buttons);

	QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	if(dialog.exec() != QDialog::Accepted)
		return { -1, "" };

	int year = yearCombo->currentText().toInt();

	std::string text = inputField->toPlainText().toStdString();

	return { year, text };
}

void ImportExpenses::addExpense(QWidget* parent, const Expense& e)
{
	auto& enveloppes = g_enveloppeManager.getEnveloppes();
	bool matched = false;

	for(auto& env : enveloppes)
	{
		for(const std::string& type : env.getTypes())
		{
			if(e.info == type)
			{
				g_enveloppeManager.addExpense(e, env);
				matched = true;
				break;
			}
		}

		if(matched)
			break;
	}

	if(!matched)
		g_enveloppeManager.addTypeAndExpense(selectEnveloppeDialog(parent, enveloppes, e), e);
}

std::string ImportExpenses::selectEnveloppeDialog(QWidget* parent, const std::vector<Enveloppe>& enveloppes, const Expense& e)
{
	QStringList names;

	for(const auto& env : enveloppes)
		names << QString::fromStdString(env.getName());

	QString dateStr = QString("%1-%2-%3")
	                  .arg(int(e.date.year()))
	                  .arg(unsigned(e.date.month()), 2, 10, QChar('0'))
	                  .arg(unsigned(e.date.day()), 2, 10, QChar('0'));

	QString label = QString("Date : %1\nMontant : %2\nInfo : %3\n\nEnveloppes :")
	                .arg(dateStr)
	                .arg(e.amount)
	                .arg(QString::fromStdString(e.info));

	QString selected;

	while(selected.isEmpty())
	{
		QInputDialog dialog(parent);
		dialog.setWindowTitle("Choisir une enveloppe");
		dialog.setLabelText(label);
		dialog.setComboBoxItems(names);
		dialog.setStyleSheet(R"(
			QLabel { color: black; background: white; }
			QComboBox { color: black; background: white; }
			QListView { color: black; background: white; }
			QLineEdit { color: black; background: white; }
			QPushButton { color: black; background: white; }
		)");
		dialog.setOption(QInputDialog::UseListViewForComboBoxItems);

		if(dialog.exec() == QDialog::Accepted)
			selected = dialog.textValue();
	}

	return selected.toStdString();
}
