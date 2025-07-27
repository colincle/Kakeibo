#include "EnveloppesTransfer.hpp"
#include "Assets.hpp"
#include "Globals.hpp"

#include <functional>
#include <tuple>

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

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
	QString style = QString(R"(
		QDialog {
			background-color: #242F32;
			color: #E1E1E2;
			font-family: "Helvetica Neue";
		}
		QLabel {
			color: #E1E1E2;
		}
		QLineEdit, QComboBox {
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
	)").arg(DOWN_ICON);

	dialog.setStyleSheet(style);
}

TransferUi EnveloppesTransfer::createUiElements(QDialog& dialog, const std::vector<Enveloppe>& envs)
{
	QComboBox* fromBox = createComboBox(dialog);
	QComboBox* toBox = createComboBox(dialog);

	QDialogButtonBox* buttons;
	QPushButton* okButton;
	QPushButton* cancelButton;
	std::tie(buttons, okButton, cancelButton) = createDialogButtons(dialog);

	TransferUi ui
	{
		fromBox,
		toBox,
		new QLineEdit(&dialog),
		new QPushButton("Remplir jusqu’à l’objectif \n目標まで追加", &dialog),
		new QLabel(&dialog),
		buttons,
		envs,
		okButton,
		cancelButton
	};
	return ui;
}

QComboBox* EnveloppesTransfer::createComboBox(QDialog& dialog)
{
	QComboBox* box = new QComboBox(&dialog);
	QListView* view = new QListView(&dialog);
	view->setStyleSheet(R"(
		QListView {
			background-color: #1B272A;
			color: #E1E1E2;
			padding: 0px;
			margin: 0px;
			border: none;
		}
		QListView::item:selected {
			background-color: #2F3D41;
			color: #E1E1E2;
		}
	)");
	box->setView(view);
	return box;
}

std::tuple<QDialogButtonBox*, QPushButton*, QPushButton*> EnveloppesTransfer::createDialogButtons(QDialog& dialog)
{
	auto* box = new QDialogButtonBox(Qt::Horizontal, &dialog);
	auto* ok = new QPushButton("OK 確定", &dialog);
	auto* cancel = new QPushButton("Annuler キャンセル", &dialog);
	box->addButton(ok, QDialogButtonBox::AcceptRole);
	box->addButton(cancel, QDialogButtonBox::RejectRole);

	QObject::connect(box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	return { box, ok, cancel };
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

void EnveloppesTransfer::buildLayout(QDialog& dialog, TransferUi& ui)
{
	QFormLayout* layout = createFormLayout(dialog);
	styleWidgets(ui);
	addComboBoxes(layout, ui);
	addAmountRow(dialog, layout, ui);
	layout->addRow("", ui.previewLabel);
	layout->addWidget(ui.buttons);
}

QFormLayout* EnveloppesTransfer::createFormLayout(QDialog& dialog)
{
	QFormLayout* layout = new QFormLayout(&dialog);
	layout->setLabelAlignment(Qt::AlignLeft);
	layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
	layout->setSpacing(10);
	return layout;
}

void EnveloppesTransfer::styleWidgets(TransferUi& ui)
{
	QString comboStyle = R"(
		background-color: #1B272A;
		color: #E1E1E2;
		border: 1px solid #2F3D41;
		border-radius: 4px;
		padding: 4px;
	)";
	QString inputStyle = R"(
		background-color: #1B272A;
		color: #E1E1E2;
		border: 1px solid #2F3D41;
		border-radius: 4px;
		padding: 4px;
	)";
	QString buttonStyle = R"(
		background-color: #1B272A;
		color: #E1E1E2;
		border: none;
		padding: 6px 12px;
		border-radius: 4px;
	)";

	ui.fromBox->setStyleSheet(comboStyle);
	ui.toBox->setStyleSheet(comboStyle);
	ui.amountEdit->setStyleSheet(inputStyle);
	ui.previewLabel->setStyleSheet("color: #E1E1E2;");
	ui.fillButton->setStyleSheet(buttonStyle);
}

void EnveloppesTransfer::addComboBoxes(QFormLayout* layout, TransferUi& ui)
{
	layout->addRow("De\n送金元:", ui.fromBox);
	layout->addRow("À\n送金先:", ui.toBox);
}

void EnveloppesTransfer::addAmountRow(QDialog& dialog, QFormLayout* layout, TransferUi& ui)
{
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setHorizontalSpacing(10);
	grid->addWidget(ui.amountEdit, 0, 0);
	grid->addWidget(ui.fillButton, 0, 1);

	QWidget* rowWidget = new QWidget(&dialog);
	rowWidget->setLayout(grid);
	layout->addRow("Amount:", rowWidget);
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

	QObject::connect(ui.fillButton, &QPushButton::clicked, makeFillHandler(ui));
	QObject::connect(ui.buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(ui.buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	ui.okButton->setEnabled(false);
}

std::function<void()> EnveloppesTransfer::makeFillHandler(TransferUi& ui)
{
	return [ =, &ui ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();

		if(fromRaw < 0 || toRaw < 0)
			return;

		size_t fromIdx = static_cast<size_t>(fromRaw);
		size_t toIdx = static_cast<size_t>(toRaw);
		int val = fillToGoal(ui.enveloppes[fromIdx], ui.enveloppes[toIdx]);

		if(val > 0)
			ui.amountEdit->setText(QString::number(val));
	};
}

auto EnveloppesTransfer::makeUpdateOk(const TransferUi& ui) -> std::function<void()>
{
	return [ = ]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw = ui.toBox->currentIndex();

		if(fromRaw < 0 || toRaw < 0 || fromRaw == toRaw)
			return ui.okButton->setEnabled(false);

		size_t fromIdx = static_cast<size_t>(fromRaw);
		int amount = ui.amountEdit->text().toInt();
		bool valid = !ui.amountEdit->text().isEmpty()
		             && amount > 0
		             && amount <= ui.enveloppes[fromIdx].getAmount();
		ui.okButton->setEnabled(valid);
	};
}

auto EnveloppesTransfer::makeUpdateFill(const TransferUi& ui) -> std::function<void()>
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

auto EnveloppesTransfer::makeUpdatePreview(const TransferUi& ui) -> std::function<void()>
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
		fn.replace('\n', ' ');
		tn.replace('\n', ' ');
		ui.previewLabel->setText(QString("%1: %2 → %3\n%4: %5 → %6")
		                         .arg(fn).arg(ui.enveloppes[fromIdx].getAmount()).arg(n.newFrom)
		                         .arg(tn).arg(ui.enveloppes[toIdx].getAmount()).arg(n.newTo));
	};
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
