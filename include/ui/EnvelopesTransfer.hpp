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

class Envelope;

struct NewAmounts
{
	int newFrom;
	int newTo;
};

struct TransferUi
{
	QComboBox            *fromBox;
	QComboBox            *toBox;
	QLineEdit            *amountEdit;
	QPushButton          *fillButton;
	QLabel               *previewLabel;
	QDialogButtonBox     *buttons;
	std::vector<Envelope> envelopes;
	QPushButton          *okButton;
	QPushButton          *cancelButton;
};

class EnvelopesTransfer final
{
  private:
	EnvelopesTransfer()                                     = delete;
	~EnvelopesTransfer()                                    = delete;
	EnvelopesTransfer(const EnvelopesTransfer &)            = delete;
	EnvelopesTransfer &operator=(const EnvelopesTransfer &) = delete;
	EnvelopesTransfer(EnvelopesTransfer &&)                 = delete;
	EnvelopesTransfer &operator=(EnvelopesTransfer &&)      = delete;

	static void       setupDialogStyle(QDialog &dialog);
	static TransferUi createUiElements(QDialog &dialog, const std::vector<Envelope> &envs);
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

	static std::function<void()> makeUpdateOk(const TransferUi &ui);
	static std::function<void()> makeUpdatePreview(const TransferUi &ui);

  public:
	static void       transfer(QWidget *parent);
	static NewAmounts newAmounts(const Envelope &from, const Envelope &to, int amount);
};
