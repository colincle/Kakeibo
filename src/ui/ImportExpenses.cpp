#include "ImportExpenses.hpp"
#include "Assets.hpp"
#include "Envelope.hpp"
#include "Globals.hpp"
#include "Parser.hpp"
#include "Theme.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLabel>
#include <QLocale>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <chrono>
#include <string>
#include <vector>

void ImportExpenses::import(QWidget *parent)
{
	auto [yearInt, data] = showImportDialog(parent);

	if ( yearInt < 0 || data.empty() )
		return;

	std::chrono::year    year {yearInt};
	std::vector<Expense> expenses = Parser::parseExpenses(data, year);

	for ( const Expense &e : expenses )
		addExpense(parent, e);
}

std::pair<int, std::string> ImportExpenses::showImportDialog(QWidget *parent)
{
	QDialog dialog(parent);
	dialog.setStyleSheet(setDialogStyleSheet());

	QVBoxLayout *layout = new QVBoxLayout(&dialog);

	QComboBox      *yearCombo  = createYearSelector(&dialog, layout);
	QPlainTextEdit *inputField = createTextInput(&dialog, layout);
	addDialogButtons(&dialog, layout);

	if ( dialog.exec() != QDialog::Accepted )
		return {-1, ""};

	return {
	    yearCombo->currentText().toInt(),
	    inputField->toPlainText().toStdString()};
}

QComboBox *ImportExpenses::createYearSelector(QWidget *parent, QVBoxLayout *layout)
{
	auto label = new QLabel("Année\n年 :", parent);
	layout->addWidget(label);

	QComboBox *combo = new QComboBox(parent);

	for ( int y = 1900; y <= 2100; ++y )
		combo->addItem(QString::number(y));

	combo->setCurrentText(QString::number(QDate::currentDate().year()));
	layout->addWidget(combo);
	return combo;
}

QPlainTextEdit *ImportExpenses::createTextInput(QWidget *parent, QVBoxLayout *layout)
{
	auto label = new QLabel("Données\nデータ :", parent);
	layout->addWidget(label);

	QPlainTextEdit *input = new QPlainTextEdit(parent);
	input->setPlaceholderText("Collez les données de dépenses\n支出データを貼り付けてください");
	input->setMinimumHeight(300);
	layout->addWidget(input);
	return input;
}

void ImportExpenses::addDialogButtons(QDialog *dialog, QVBoxLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(dialog);
	buttons->addButton(new QPushButton("OK", dialog), QDialogButtonBox::AcceptRole);
	buttons->addButton(new QPushButton("Annuler キャンセル", dialog), QDialogButtonBox::RejectRole);
	layout->addWidget(buttons);

	QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
}

QString ImportExpenses::setDialogStyleSheet()
{
	return Theme::dialogStyle(DOWN_ICON);
}

void ImportExpenses::addExpense(QWidget *parent, const Expense &e)
{
	auto &envelopes = g_envelopeManager.getEnvelopes();
	bool  matched   = false;

	if ( e.amount > 0 )
	{
		g_envelopeManager.addExpense(e, g_envelopeManager.getIncomeEnvelope());
		return;
	}

	for ( auto &env : envelopes )
	{
		for ( const std::string &type : env.getTypes() )
		{
			if ( e.info == type )
			{
				g_envelopeManager.addExpense(e, env);
				matched = true;
				break;
			}
		}

		if ( matched )
			break;
	}

	for ( const std::string &type : g_envelopeManager.getCreditEnvelope().getTypes() )
	{
		if ( e.info == type )
		{
			g_envelopeManager.addExpense(e, g_envelopeManager.getCreditEnvelope());
			matched = true;
			break;
		}
	}

	if ( !matched )
	{
		bool        rememberType = false;
		std::string selected     = selectEnvelopeDialog(parent, envelopes, e, rememberType);
		g_envelopeManager.addTypeAndExpense(selected, e, rememberType);
	}
}

std::string ImportExpenses::selectEnvelopeDialog(QWidget *parent, const std::vector<Envelope> &envelopes, const Expense &e, bool &rememberType)
{
	QStringList names;

	for ( const auto &env : envelopes )
		names << QString::fromStdString(env.getName());

	names << QString::fromStdString(g_envelopeManager.getCreditEnvelope().getName());

	QString selected;
	QString label = buildDialogLabel(e);

	while ( selected.isEmpty() )
	{
		QInputDialog dialog(parent, Qt::WindowFlags());
		dialog.setOption(QInputDialog::NoButtons);
		dialog.setWindowTitle("Choisir une enveloppe / 封筒を選ぶ");
		dialog.setLabelText(label);
		dialog.setComboBoxItems(names);
		dialog.setOption(QInputDialog::UseListViewForComboBoxItems);
		dialog.setStyleSheet(setDialogStyleSheet());

		QCheckBox        *check   = new QCheckBox("Se souvenir de ce choix この選択を記憶する");
		QDialogButtonBox *buttons = createDialogButtons(&dialog);
		QVBoxLayout      *layout  = qobject_cast<QVBoxLayout *>(dialog.layout());

		if ( layout )
		{
			layout->addWidget(check);
			layout->addWidget(buttons);
		}

		QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
		QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

		if ( dialog.exec() == QDialog::Accepted )
		{
			selected     = dialog.textValue();
			rememberType = check->isChecked();
		}
	}

	return selected.toStdString();
}

QString ImportExpenses::buildDialogLabel(const Expense &e)
{
	QString dateStr = QString("%1-%2-%3")
	                      .arg(int(e.date.year()))
	                      .arg(unsigned(e.date.month()), 2, 10, QChar('0'))
	                      .arg(unsigned(e.date.day()), 2, 10, QChar('0'));

	double  amountValue = static_cast<double>(e.amount);
	QString number      = QLocale(QLocale::Japanese, QLocale::Japan).toString(qAbs(amountValue));
	QString amountStr   = e.amount < 0 ? QString("¥-%1").arg(number) : QString("¥%1").arg(number);

	return QString("%1\n%2\n%3")
	    .arg(dateStr)
	    .arg(amountStr)
	    .arg(QString::fromStdString(e.info));
}

QDialogButtonBox *ImportExpenses::createDialogButtons(QDialog *parent)
{
	auto *box    = new QDialogButtonBox(parent);
	auto *ok     = new QPushButton("OK", parent);
	auto *cancel = new QPushButton("Annuler キャンセル", parent);
	box->addButton(ok, QDialogButtonBox::AcceptRole);
	box->addButton(cancel, QDialogButtonBox::RejectRole);

	QString style = setDialogStyleSheet();
	ok->setStyleSheet(style);
	cancel->setStyleSheet(style);
	return box;
}
