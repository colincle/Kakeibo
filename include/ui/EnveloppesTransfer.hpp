#pragma once

#include <string>
#include <vector>

class QWidget;
class QDialog;
class QComboBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QDialogButtonBox;
class QFormLayout;
class QHBoxLayout;

class Enveloppe;

struct NewAmounts
{
	int newFrom;
	int newTo;
};

struct TransferUi
{
	QComboBox* fromBox;
	QComboBox* toBox;
	QLineEdit* amountEdit;
	QPushButton* fillButton;
	QLabel* previewLabel;
	QDialogButtonBox* buttons;
	const std::vector<Enveloppe>& enveloppes;
};

class EnveloppesTransfer final
{
private:
	EnveloppesTransfer() = delete;
	~EnveloppesTransfer() = delete;
	EnveloppesTransfer(const EnveloppesTransfer&) = delete;
	EnveloppesTransfer& operator=(const EnveloppesTransfer&) = delete;
	EnveloppesTransfer(EnveloppesTransfer&&) = delete;
	EnveloppesTransfer& operator=(EnveloppesTransfer&&) = delete;

	static void setupDialogStyle(QDialog& dialog);
	static TransferUi createUiElements(QDialog& dialog, const std::vector<Enveloppe>& envs);
	static void fillComboBoxes(TransferUi& ui);
	static void setupConnections(TransferUi& ui, QDialog& dialog);
	static void buildLayout(QDialog& dialog, TransferUi& ui);
	static void applyTransfer(const TransferUi& ui);

public:
	static void transfer(QWidget* parent);
	static int fillToGoal(Enveloppe from, Enveloppe to);
	static NewAmounts newAmounts(Enveloppe from, Enveloppe to, int amount);
};
