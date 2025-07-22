#include "MainView.hpp"
#include "Globals.hpp"
#include "MenuBar.hpp"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QIntValidator>
#include <QDialogButtonBox>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

MainView::MainView(MenuBar* menuBar, QWidget* parent) : QWidget(parent)
{
	connect(menuBar, &MenuBar::menuButtonClicked, this, &MainView::handleMenuAction);

	gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(10, 0, 10, 0);
	setLayout(gridLayout);

	showEnveloppes();
}

void MainView::handleMenuAction(int index)
{
	switch (index)
	{
		case 0:
			// Handle "Importer des dépenses"
			break;
		case 1:
			// Handle "Transférer entre enveloppes"
			break;
		case 2:
			addEnveloppe();
			break;
		case 3:
			// Handle "Historique"
			break;
		case 4:
			// Handle "Statistiques"
			break;
		default:
			break;
	}
}

void MainView::showEnveloppes()
{
	if (!gridLayout)
	{
		gridLayout = new QGridLayout(this);
		setLayout(gridLayout);
	}
	else
	{
		QLayoutItem* item;
		while ((item = gridLayout->takeAt(0)))
		{
			delete item->widget();
			delete item;
		}
	}

	int row = 0;
	for (const auto& env : g_enveloppeManager.enveloppes)
	{
		QString info = QString("Nom: %1\nMontant: %2\nMaximum: %3\nObjectif: %4\nÉpargne: %5")
			.arg(QString::fromStdString(env.getName()))
			.arg(env.getAmount())
			.arg(env.getMaxAmount())
			.arg(env.getGoal())
			.arg(env.isSavings() ? "Oui" : "Non");

		QLabel* label = new QLabel(info);
		label->setStyleSheet("border: 1px solid #ccc; padding: 10px;");
		gridLayout->addWidget(label, row / 2, row % 2);
		++row;
	}
}

void MainView::addEnveloppe()
{
	QDialog dialog(this);
	dialog.setWindowTitle("Ajouter une enveloppe");

	QFormLayout* formLayout = new QFormLayout(&dialog);

	QLineEdit* nameInput = new QLineEdit(&dialog);
	QLineEdit* amountInput = new QLineEdit(&dialog);
	QLineEdit* maxAmountInput = new QLineEdit(&dialog);
	QLineEdit* goalInput = new QLineEdit(&dialog);
	QCheckBox* savingsCheck = new QCheckBox("Épargne", &dialog);

	QIntValidator* validator = new QIntValidator(0, 100000000, &dialog);
	amountInput->setValidator(validator);
	maxAmountInput->setValidator(validator);
	goalInput->setValidator(validator);

	formLayout->addRow("Nom", nameInput);
	formLayout->addRow("Montant initial", amountInput);
	formLayout->addRow("Montant max", maxAmountInput);
	formLayout->addRow("Objectif", goalInput);
	formLayout->addRow(savingsCheck);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
	formLayout->addWidget(buttons);
	connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	if (dialog.exec() == QDialog::Accepted)
	{
		QString name = nameInput->text();
		int amount = amountInput->text().toInt();
		int maxAmount = maxAmountInput->text().toInt();
		int goal = goalInput->text().toInt();
		bool savings = savingsCheck->isChecked();

		g_enveloppeManager.addEnveloppe(name.toStdString(), amount, maxAmount, goal, savings);
		showEnveloppes();
	}
}

