#include "EnveloppesTransfer.hpp"
#include "Globals.hpp"

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

void EnveloppesTransfer::transfer(QWidget* parent)
{
	QDialog dialog(parent);
	setupDialogStyle(dialog);

	const auto& envs = g_enveloppeManager.getEnveloppes();
	TransferUi ui = createUiElements(dialog, envs);
	fillComboBoxes(ui);
	setupConnections(ui, dialog);
	buildLayout(dialog, ui);

	if(dialog.exec() == QDialog::Accepted)
		applyTransfer(ui);
}

void EnveloppesTransfer::setupDialogStyle(QDialog& dialog)
{
	dialog.setWindowTitle("Transfer Enveloppe");
	dialog.setStyleSheet(R"(
		QLabel { color: black; }
		QPushButton { color: black; }
		QDialogButtonBox QPushButton { color: black; }
		QPushButton:disabled { color: gray; }
	)");
}

TransferUi EnveloppesTransfer::createUiElements(QDialog& dialog, const std::vector<Enveloppe>& envs)
{
	TransferUi ui
	{
		new QComboBox(&dialog),
		new QComboBox(&dialog),
		new QLineEdit(&dialog),
		new QPushButton("Fill to goal", &dialog),
		new QLabel(&dialog),
		new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog),
		envs
	};
	ui.fromBox->setStyleSheet("color: black;");
	ui.toBox->setStyleSheet("color: black;");
	ui.amountEdit->setStyleSheet("color: black;");
	ui.fillButton->setStyleSheet("color: black;");
	ui.previewLabel->setStyleSheet("color: black;");
	return ui;
}

void EnveloppesTransfer::fillComboBoxes(TransferUi& ui)
{
	for(const auto& env : ui.enveloppes)
	{
		QString name = QString::fromStdString(env.getName());
		ui.fromBox->addItem(name);
		ui.toBox->addItem(name);
	}
}

static auto makeUpdateOk(const TransferUi& ui)
{
	return [ = ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();

		if(fromRaw < 0 || toRaw < 0 || fromRaw == toRaw)
			return ui.buttons->button(QDialogButtonBox::Ok)->setEnabled(false);

		size_t fromIdx = static_cast<size_t>(fromRaw);
		int amount = ui.amountEdit->text().toInt();
		bool valid = !ui.amountEdit->text().isEmpty()
		             && amount > 0
		             && amount <= ui.enveloppes[fromIdx].getAmount();
		ui.buttons->button(QDialogButtonBox::Ok)->setEnabled(valid);
	};
}

static auto makeUpdateFill(const TransferUi& ui)
{
	return [ = ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();

		if(fromRaw < 0 || toRaw < 0 || fromRaw == toRaw)
			return ui.fillButton->setEnabled(false);

		size_t fromIdx = static_cast<size_t>(fromRaw);
		size_t toIdx = static_cast<size_t>(toRaw);
		ui.fillButton->setEnabled(
		    EnveloppesTransfer::fillToGoal(ui.enveloppes[fromIdx], ui.enveloppes[toIdx]) > 0);
	};
}

static auto makeUpdatePreview(const TransferUi& ui)
{
	return [ = ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();
		int amount = ui.amountEdit->text().toInt();

		if(fromRaw < 0 || toRaw < 0 || fromRaw == toRaw || amount <= 0)
			return ui.previewLabel->clear();

		size_t fromIdx = static_cast<size_t>(fromRaw);
		size_t toIdx = static_cast<size_t>(toRaw);

		NewAmounts n = EnveloppesTransfer::newAmounts(
		                   ui.enveloppes[fromIdx], ui.enveloppes[toIdx], amount);
		QString fn = QString::fromStdString(ui.enveloppes[fromIdx].getName());
		QString tn = QString::fromStdString(ui.enveloppes[toIdx].getName());
		ui.previewLabel->setText(QString("%1: %2 → %3\n%4: %5 → %6")
		                         .arg(fn).arg(ui.enveloppes[fromIdx].getAmount()).arg(n.newFrom)
		                         .arg(tn).arg(ui.enveloppes[toIdx].getAmount()).arg(n.newTo));
	};
}

void EnveloppesTransfer::setupConnections(TransferUi& ui, QDialog& dialog)
{
	auto updateOk = makeUpdateOk(ui);
	auto updateFill = makeUpdateFill(ui);
	auto updatePreview = makeUpdatePreview(ui);

	QObject::connect(ui.fromBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateOk);
	QObject::connect(ui.toBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateOk);
	QObject::connect(ui.amountEdit, &QLineEdit::textChanged, updateOk);

	QObject::connect(ui.fromBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateFill);
	QObject::connect(ui.toBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateFill);
	updateFill();

	QObject::connect(ui.fromBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePreview);
	QObject::connect(ui.toBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePreview);
	QObject::connect(ui.amountEdit, &QLineEdit::textChanged, updatePreview);
	updatePreview();

	QObject::connect(ui.fillButton, &QPushButton::clicked, [ = ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();

		if(fromRaw < 0 || toRaw < 0)
			return;

		size_t fromIdx = static_cast<size_t>(fromRaw);
		size_t toIdx = static_cast<size_t>(toRaw);

		int val = EnveloppesTransfer::fillToGoal(ui.enveloppes[fromIdx], ui.enveloppes[toIdx]);

		if(val > 0)
			ui.amountEdit->setText(QString::number(val));
	});

	QObject::connect(ui.buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(ui.buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	ui.buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void EnveloppesTransfer::buildLayout(QDialog& dialog, TransferUi& ui)
{
	QFormLayout* layout = new QFormLayout(&dialog);
	layout->addRow("From:", ui.fromBox);
	layout->addRow("To:", ui.toBox);
	QHBoxLayout* amtRow = new QHBoxLayout();
	amtRow->addWidget(ui.amountEdit);
	amtRow->addWidget(ui.fillButton);
	layout->addRow("Amount:", amtRow);
	layout->addRow("Preview:", ui.previewLabel);
	layout->addWidget(ui.buttons);
}

void EnveloppesTransfer::applyTransfer(const TransferUi& ui)
{
	std::string from = ui.fromBox->currentText().toStdString();
	std::string to = ui.toBox->currentText().toStdString();
	int amount = ui.amountEdit->text().toInt();
	g_enveloppeManager.transfer(from, to, amount);
}

int EnveloppesTransfer::fillToGoal(Enveloppe from, Enveloppe to)
{
	int amount;

	if(to.isSavings())
		amount = to.getGoal();
	else
		amount = std::max(0, to.getGoal() - to.getAmount());

	if(amount > from.getAmount())
		return 0;

	return amount;
}

NewAmounts EnveloppesTransfer::newAmounts(Enveloppe from, Enveloppe to, int amount)
{
	NewAmounts newAmounts;

	newAmounts.newFrom = from.getAmount() - amount;
	newAmounts.newTo = to.getAmount() + amount;
	return newAmounts;
}
