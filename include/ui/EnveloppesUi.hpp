#pragma once

#include "Enveloppe.hpp"

#include <QWidget>

class QGridLayout;
class QLineEdit;
class QCheckBox;
class QFormLayout;
class QTimer;
class QLabel;
class QScrollArea;
class QString;

class EnveloppesUi : public QWidget
{
	Q_OBJECT

public:
	EnveloppesUi(QWidget* parent = nullptr);
	void showEnveloppes();

	void addEnveloppe();

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	struct EnveloppeFormFields
	{
		QLineEdit* nameFrInput = nullptr;
		QLineEdit* nameJpInput = nullptr;
		QLineEdit* nameInput = nullptr;
		QLineEdit* amountInput = nullptr;
		QLineEdit* maxAmountInput = nullptr;
		QLineEdit* goalInput = nullptr;
		QCheckBox* savingsCheck = nullptr;
	};

	QGridLayout* gridLayout = nullptr;
	QTimer* resizeDebounceTimer = nullptr;

	void clearEnveloppes();
	void createFormInputs(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f);
	void addDialogButtons(QDialog* dialog, QFormLayout* layout);
	void handleEnveloppeSubmission(const EnveloppeFormFields& f);
	void createFields(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f);
	QWidget* createCard(const Enveloppe& env);
	QWidget* createProgressLabel(QString barColor, int percent);
	QWidget* createGoalMaxLabel(const Enveloppe& env);
	QString getProgressBarColor(int percent);
	QWidget* createCardButtons(const Enveloppe& env);
	QScrollArea* createScrollArea(QWidget* content);
};
