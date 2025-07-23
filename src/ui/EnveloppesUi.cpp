#include "EnveloppesUi.hpp"
#include "Globals.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QIntValidator>
#include <QString>

EnveloppesUi::EnveloppesUi(QWidget* parent) : QWidget(parent)
{
	gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(10, 10, 10, 10);
	setLayout(gridLayout);
	showEnveloppes();
}

void EnveloppesUi::showEnveloppes() //refactor
{
	clearEnveloppes();

	int row = 0;

	for(const auto& env : g_enveloppeManager.getEnveloppes())
	{
		QString info = QString("Nom: %1\nMontant: %2\nMaximum: %3\nObjectif: %4\nÉpargne: %5")
		               .arg(QString::fromStdString(env.getName()))
		               .arg(env.getAmount())
		               .arg(env.getMaxAmount())
		               .arg(env.getGoal())
		               .arg(env.isSavings() ? "Oui" : "Non");

		QWidget* card = new QWidget;
		card->setStyleSheet(R"(
			background-color: white;
			border: 1px solid #ccc;
			border-radius: 8px;
			padding: 10px;
		)");

		QVBoxLayout* cardLayout = new QVBoxLayout(card);
		cardLayout->setContentsMargins(10, 10, 10, 10);

		QLabel* label = new QLabel(info);
		label->setWordWrap(true);
		cardLayout->addWidget(label);

		gridLayout->addWidget(card, row / 2, row % 2);

		label->setStyleSheet("border: 1px solid #ccc; padding: 10px; color: black;");
		gridLayout->addWidget(label, row / 2, row % 2);
		++row;
	}
}

void EnveloppesUi::clearEnveloppes()
{
	if(!gridLayout)
	{
		gridLayout = new QGridLayout(this);
		setLayout(gridLayout);
	}
	else
	{
		QLayoutItem* item;

		while((item = gridLayout->takeAt(0)))
		{
			delete item->widget();
			delete item;
		}
	}
}

void EnveloppesUi::addEnveloppe()
{
	QDialog dialog(this);
	dialog.setWindowTitle("Ajouter une enveloppe");

	QFormLayout* formLayout = new QFormLayout(&dialog);
	EnveloppeFormFields fields;

	createFormInputs(&dialog, formLayout, fields);
	addDialogButtons(&dialog, formLayout);

	if(dialog.exec() == QDialog::Accepted)
		handleEnveloppeSubmission(fields);
}

void EnveloppesUi::createFormInputs(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f) //refactor
{
	f.nameInput       = new QLineEdit(dialog);
	f.amountInput     = new QLineEdit(dialog);
	f.maxAmountInput  = new QLineEdit(dialog);
	f.goalInput       = new QLineEdit(dialog);
	f.savingsCheck    = new QCheckBox("Épargne", dialog);

	QIntValidator* validator = new QIntValidator(0, 100000000, dialog);
	f.amountInput->setValidator(validator);
	f.maxAmountInput->setValidator(validator);
	f.goalInput->setValidator(validator);

	layout->addRow("Nom", f.nameInput);
	layout->addRow("Montant initial", f.amountInput);
	layout->addRow("Montant max", f.maxAmountInput);
	layout->addRow("Objectif", f.goalInput);
	layout->addRow(f.savingsCheck);

	// Black text inside the input fields
	QLineEdit* edits[] = { f.nameInput, f.amountInput, f.maxAmountInput, f.goalInput };
	for (QLineEdit* edit : edits)
	{
		edit->setStyleSheet("color: black; background-color: white;");
	}

	// Black text for the field labels
	QLabel* labels[] = {
		qobject_cast<QLabel*>(layout->labelForField(f.nameInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.amountInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.maxAmountInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.goalInput))
	};

	for (QLabel* label : labels)
		if (label)
			label->setStyleSheet("color: black;");

	f.savingsCheck->setStyleSheet("color: black;");
}


void EnveloppesUi::addDialogButtons(QDialog* dialog, QFormLayout* layout)
{
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
	buttons->setStyleSheet("color: black;");
	layout->addWidget(buttons);

	connect(buttons, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
}

void EnveloppesUi::handleEnveloppeSubmission(const EnveloppeFormFields& f)
{
	QString name = f.nameInput->text();
	int amount = f.amountInput->text().toInt();
	int maxAmount = f.maxAmountInput->text().toInt();
	int goal = f.goalInput->text().toInt();
	bool savings = f.savingsCheck->isChecked();

	g_enveloppeManager.addEnveloppe(name.toStdString(), amount, maxAmount, goal, savings);
	showEnveloppes();
}
