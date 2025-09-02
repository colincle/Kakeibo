#pragma once

#include <functional>
#include <string>
#include <tuple>
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
class QListView;

class Enveloppe;

struct NewAmounts
{
	int newFrom;
	int newTo;
};

struct TransferUi
{
	QComboBox             *fromBox;
	QComboBox             *toBox;
	QLineEdit             *amountEdit;
	QPushButton           *fillButton;
	QLabel                *previewLabel;
	QDialogButtonBox      *buttons;
	std::vector<Enveloppe> enveloppes;
	QPushButton           *okButton;
	QPushButton           *cancelButton;
};

class EnveloppesTransfer final
{
  private:
	EnveloppesTransfer()                                      = delete;
	~EnveloppesTransfer()                                     = delete;
	EnveloppesTransfer(const EnveloppesTransfer &)            = delete;
	EnveloppesTransfer &operator=(const EnveloppesTransfer &) = delete;
	EnveloppesTransfer(EnveloppesTransfer &&)                 = delete;
	EnveloppesTransfer &operator=(EnveloppesTransfer &&)      = delete;

	static void       setupDialogStyle(QDialog &dialog);
	static TransferUi createUiElements(QDialog &dialog, const std::vector<Enveloppe> &envs);
	static QComboBox *createComboBox(QDialog &dialog);
	static std::tuple<QDialogButtonBox *, QPushButton *, QPushButton *>
	createDialogButtons(QDialog &dialog);

	static void fillComboBoxes(TransferUi &ui);
	static void setupConnections(TransferUi &ui, QDialog &dialog);
	static void buildLayout(QDialog &dialog, TransferUi &ui);
	static void applyTransfer(const TransferUi &ui);

	static QFormLayout *createFormLayout(QDialog &dialog);
	static void         styleWidgets(TransferUi &ui);
	static void         addComboBoxes(QFormLayout *layout, TransferUi &ui);

	static std::function<void()> makeFillHandler(TransferUi &ui);
	static std::function<void()> makeUpdateOk(const TransferUi &ui);
	static std::function<void()> makeUpdateFill(const TransferUi &ui);
	static std::function<void()> makeUpdatePreview(const TransferUi &ui);

  public:
	static void       transfer(QWidget *parent);
	static int        fillToGoal(Enveloppe from, Enveloppe to);
	static NewAmounts newAmounts(Enveloppe from, Enveloppe to, int amount);
};
