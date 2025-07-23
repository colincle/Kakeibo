#pragma once

#include <QWidget>

class QGridLayout;
class QLineEdit;
class QCheckBox;
class QFormLayout;

class EnveloppesUi : public QWidget
{
	Q_OBJECT

public:
	EnveloppesUi(QWidget* parent = nullptr);

	void addEnveloppe();

private:
	struct EnveloppeFormFields
	{
		QLineEdit* nameInput = nullptr;
		QLineEdit* amountInput = nullptr;
		QLineEdit* maxAmountInput = nullptr;
		QLineEdit* goalInput = nullptr;
		QCheckBox* savingsCheck = nullptr;
	};

	QGridLayout* gridLayout = nullptr;

	void showEnveloppes();
	void clearEnveloppes();
	void createFormInputs(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f);
	void addDialogButtons(QDialog* dialog, QFormLayout* layout);
	void handleEnveloppeSubmission(const EnveloppeFormFields& f);
};
