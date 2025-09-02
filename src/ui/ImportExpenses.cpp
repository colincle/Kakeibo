#include "ImportExpenses.hpp"
#include "Assets.hpp"
#include "Enveloppe.hpp"
#include "Globals.hpp"
#include "Parser.hpp"

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
	return QString(R"(
		QDialog {
			background-color: #242F32;
			color: #E1E1E2;
			font-family: "Helvetica Neue";
		}
		QLabel {
			color: #E1E1E2;
		}
		QLineEdit, QComboBox, QPlainTextEdit {
			background-color: #1B272A;
			color: #E1E1E2;
			border: 1px solid #444;
			border-radius: 4px;
			padding: 4px;
			min-height: 28px;
			padding-left: 6px;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 25px;
			border-left: 1px solid #444;
			background-color: #2F3D41;
		}
		QComboBox::down-arrow {
			image: url(%1);
			width: 12px;
			height: 12px;
		}
		QListView {
			background-color: #1B272A;
			color: #E1E1E2;
		}
		QListView::item:selected {
			background-color: #2F3D41;
			color: #E1E1E2;
		}
		QScrollBar:vertical {
			background: transparent;
			width: 8px;
			margin: 3px 0;
			border-radius: 4px;
		}
		QScrollBar::handle:vertical {
			background: #1B272A;
			min-height: 20px;
			border-radius: 4px;
		}
		QScrollBar::add-line:vertical,
		QScrollBar::sub-line:vertical {
			height: 0;
		}
		QScrollBar::add-page:vertical,
		QScrollBar::sub-page:vertical {
			background: none;
		}
		QPushButton {
			background-color: #1B272A;
			color: #E1E1E2;
			border: none;
			padding: 6px 12px;
			border-radius: 2px;
			min-width: 80px;
		}
		QPushButton:hover {
			background-color: #2F3D41;
		}
		QPushButton:disabled {
			color: gray;
		}
	)")
	    .arg(DOWN_ICON);
}

void ImportExpenses::addExpense(QWidget *parent, const Expense &e)
{
	auto &enveloppes = g_enveloppeManager.getEnveloppes();
	bool  matched    = false;

	if ( e.amount > 0 )
	{
		g_enveloppeManager.addExpense(e, g_enveloppeManager.getIncomeEnveloppe());
		return;
	}

	for ( auto &env : enveloppes )
	{
		for ( const std::string &type : env.getTypes() )
		{
			if ( e.info == type )
			{
				g_enveloppeManager.addExpense(e, env);
				matched = true;
				break;
			}
		}

		if ( matched )
			break;
	}

	for ( const std::string &type : g_enveloppeManager.getCreditEnveloppe().getTypes() )
	{
		if ( e.info == type )
		{
			g_enveloppeManager.addExpense(e, g_enveloppeManager.getCreditEnveloppe());
			matched = true;
			break;
		}
	}

	if ( !matched )
	{
		bool        rememberType = false;
		std::string selected     = selectEnveloppeDialog(parent, enveloppes, e, rememberType);
		g_enveloppeManager.addTypeAndExpense(selected, e, rememberType);
	}
}

std::string ImportExpenses::selectEnveloppeDialog(QWidget *parent, const std::vector<Enveloppe> &enveloppes, const Expense &e, bool &rememberType)
{
	QStringList names;

	for ( const auto &env : enveloppes )
		names << QString::fromStdString(env.getName());

	names << QString::fromStdString(g_enveloppeManager.getCreditEnveloppe().getName());

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
