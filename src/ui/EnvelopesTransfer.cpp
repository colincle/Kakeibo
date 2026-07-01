#include "EnvelopesTransfer.hpp"
#include "Assets.hpp"
#include "Globals.hpp"
#include "Theme.hpp"

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

void EnvelopesTransfer::transfer(QWidget *parent)
{
	QDialog dialog(parent);
	setupDialogStyle(dialog);

	std::vector<Envelope> envs = g_envelopeManager.getEnvelopes();
	envs.push_back(g_envelopeManager.getCreditEnvelope());
	envs.push_back(g_envelopeManager.getIncomeEnvelope());

	TransferUi ui = createUiElements(dialog, envs);
	fillComboBoxes(ui);
	setupConnections(ui, dialog);
	buildLayout(dialog, ui);

	if ( dialog.exec() == QDialog::Accepted )
		applyTransfer(ui);
}

void EnvelopesTransfer::setupDialogStyle(QDialog &dialog)
{
	dialog.setStyleSheet(Theme::dialogStyle(DOWN_ICON));
}

TransferUi EnvelopesTransfer::createUiElements(QDialog &dialog, const std::vector<Envelope> &envs)
{
	QComboBox *fromBox = createComboBox(dialog);
	QComboBox *toBox   = createComboBox(dialog);

	QDialogButtonBox *buttons;
	QPushButton      *okButton;
	QPushButton      *cancelButton;
	std::tie(buttons, okButton, cancelButton) = createDialogButtons(dialog);

	TransferUi ui {
	    fromBox,
	    toBox,
	    new QLineEdit(&dialog),
	    nullptr,
	    new QLabel(&dialog),
	    buttons,
	    envs,
	    okButton,
	    cancelButton};
	return ui;
}

QComboBox *EnvelopesTransfer::createComboBox(QDialog &dialog)
{
	QComboBox *box  = new QComboBox(&dialog);
	QListView *view = new QListView(&dialog);
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

std::tuple<QDialogButtonBox *, QPushButton *, QPushButton *> EnvelopesTransfer::createDialogButtons(QDialog &dialog)
{
	auto *box    = new QDialogButtonBox(Qt::Horizontal, &dialog);
	auto *ok     = new QPushButton("OK 確定", &dialog);
	auto *cancel = new QPushButton("Annuler キャンセル", &dialog);
	box->addButton(ok, QDialogButtonBox::AcceptRole);
	box->addButton(cancel, QDialogButtonBox::RejectRole);

	QObject::connect(box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	return {box, ok, cancel};
}

void EnvelopesTransfer::fillComboBoxes(TransferUi &ui)
{
	for ( const auto &env : ui.envelopes )
	{
		QString name = QString::fromStdString(env.getName());
		ui.fromBox->addItem(name);
		ui.toBox->addItem(name);
	}
}

void EnvelopesTransfer::buildLayout(QDialog &dialog, TransferUi &ui)
{
	QFormLayout *layout = createFormLayout(dialog);
	styleWidgets(ui);
	addComboBoxes(layout, ui);
	layout->addRow("Amount:", ui.amountEdit);
	layout->addRow("", ui.previewLabel);
	layout->addWidget(ui.buttons);
}

QFormLayout *EnvelopesTransfer::createFormLayout(QDialog &dialog)
{
	QFormLayout *layout = new QFormLayout(&dialog);
	layout->setLabelAlignment(Qt::AlignLeft);
	layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
	layout->setSpacing(10);
	return layout;
}

void EnvelopesTransfer::styleWidgets(TransferUi &ui)
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

	ui.fromBox->setStyleSheet(comboStyle);
	ui.toBox->setStyleSheet(comboStyle);
	ui.amountEdit->setStyleSheet(inputStyle);
	ui.previewLabel->setStyleSheet("color: #E1E1E2;");
}

void EnvelopesTransfer::addComboBoxes(QFormLayout *layout, TransferUi &ui)
{
	layout->addRow("De\n送金元:", ui.fromBox);
	layout->addRow("À\n送金先:", ui.toBox);
}

void EnvelopesTransfer::setupConnections(TransferUi &ui, QDialog &dialog)
{
	auto updateOk      = makeUpdateOk(ui);
	auto updatePreview = makeUpdatePreview(ui);

	QObject::connect(ui.fromBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateOk);
	QObject::connect(ui.toBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateOk);
	QObject::connect(ui.amountEdit, &QLineEdit::textChanged, updateOk);

	QObject::connect(ui.fromBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePreview);
	QObject::connect(ui.toBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePreview);
	QObject::connect(ui.amountEdit, &QLineEdit::textChanged, updatePreview);
	updatePreview();

	QObject::connect(ui.buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(ui.buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	ui.okButton->setEnabled(false);
}

auto EnvelopesTransfer::makeUpdateOk(const TransferUi &ui) -> std::function<void()>
{
	return [=]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw   = ui.toBox->currentIndex();

		if ( fromRaw < 0 || toRaw < 0 || fromRaw == toRaw )
			return ui.okButton->setEnabled(false);

		size_t fromIdx = static_cast<size_t>(fromRaw);
		int    amount  = ui.amountEdit->text().toInt();
		bool   valid   = !ui.amountEdit->text().isEmpty() && amount > 0 && amount <= ui.envelopes[fromIdx].getAmount();
		ui.okButton->setEnabled(valid);
	};
}

auto EnvelopesTransfer::makeUpdatePreview(const TransferUi &ui) -> std::function<void()>
{
	return [=]()
	{
		int fromRaw = ui.fromBox->currentIndex();
		int toRaw   = ui.toBox->currentIndex();
		int amount  = ui.amountEdit->text().toInt();

		if ( fromRaw < 0 || toRaw < 0 || fromRaw == toRaw || amount <= 0 )
			return ui.previewLabel->clear();

		size_t fromIdx = static_cast<size_t>(fromRaw);
		size_t toIdx   = static_cast<size_t>(toRaw);

		NewAmounts n = EnvelopesTransfer::newAmounts(
		    ui.envelopes[fromIdx], ui.envelopes[toIdx], amount);
		QString fn = QString::fromStdString(ui.envelopes[fromIdx].getName());
		QString tn = QString::fromStdString(ui.envelopes[toIdx].getName());
		fn.replace('\n', ' ');
		tn.replace('\n', ' ');
		ui.previewLabel->setText(QString("%1: %2 → %3\n%4: %5 → %6")
		                             .arg(fn)
		                             .arg(ui.envelopes[fromIdx].getAmount())
		                             .arg(n.newFrom)
		                             .arg(tn)
		                             .arg(ui.envelopes[toIdx].getAmount())
		                             .arg(n.newTo));
	};
}

void EnvelopesTransfer::applyTransfer(const TransferUi &ui)
{
	std::string from   = ui.fromBox->currentText().toStdString();
	std::string to     = ui.toBox->currentText().toStdString();
	int         amount = ui.amountEdit->text().toInt();
	g_envelopeManager.transfer(from, to, amount);
}

NewAmounts EnvelopesTransfer::newAmounts(const Envelope &from, const Envelope &to, int amount)
{
	NewAmounts newAmounts;
	newAmounts.newFrom = from.getAmount() - amount;
	newAmounts.newTo   = to.getAmount() + amount;
	return newAmounts;
}
