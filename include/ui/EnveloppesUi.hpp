#pragma once

#include "Enveloppe.hpp"

#include <QList>
#include <QString>
#include <QWidget>

class QCheckBox;
class QDialog;
class QFormLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QTimer;
class QVBoxLayout;

class EnveloppesUi : public QWidget
{
	Q_OBJECT

  signals:
	void updateNeeded();

  public:
	explicit EnveloppesUi(QWidget *parent = nullptr);
	void             showEnveloppes();
	void             addEnveloppe(std::string name);
	QList<QWidget *> getCloudCardWidgets() { return cloudCardWidgets; }

  protected:
	void resizeEvent(QResizeEvent *event) override;

  private:
	struct EnveloppeFormFields
	{
		QLineEdit *nameFrInput    = nullptr;
		QLineEdit *nameJpInput    = nullptr;
		QLineEdit *nameInput      = nullptr;
		QLineEdit *amountInput    = nullptr;
		QLineEdit *maxAmountInput = nullptr;
		QLineEdit *goalInput      = nullptr;
		QCheckBox *savingsCheck   = nullptr;
	};

	QGridLayout     *gridLayout          = nullptr;
	QTimer          *resizeDebounceTimer = nullptr;
	QScrollArea     *scrollArea          = nullptr;
	QWidget         *scrollContent       = nullptr;
	QList<QWidget *> cloudCardWidgets;

	void clearGrid();
	int  computeColumnCount() const;
	void populateGrid(int columnCount);
	void restoreScroll(int savedScroll);

	QWidget             *createCard(const Enveloppe &env);
	QVBoxLayout         *setupCardLayout(QWidget *card);
	void                 addCardContent(QVBoxLayout *layout, const Enveloppe &env, const QString &barColor, int percent);
	QWidget             *createCardButtons(const Enveloppe &env);
	QList<QPushButton *> createCardButtonList(const Enveloppe &env);
	void                 connectCardButtons(const Enveloppe &env, const QList<QPushButton *> &buttons);
	QWidget             *createProgressLabel(QString barColor, int percent);
	QWidget             *createGoalMaxLabel(const Enveloppe &env);
	QString              getProgressBarColor(int percent, const Enveloppe &env);

	void deleteEnveloppe(Enveloppe env);
	void showNonEmptyWarning();
	bool confirmDeletion();
	void addDialogButtons(QDialog *dialog, QFormLayout *layout);
	void handleEnveloppeSubmission(const EnveloppeFormFields &f, std::string name);
	void createFields(QDialog *dialog, QFormLayout *layout, EnveloppeFormFields &f);
	void setupEnveloppeDialog(QDialog &dialog);
	void preFillFieldsFromName(const std::string &name, EnveloppeFormFields &fields);

	QScrollArea *createScrollArea(QWidget *content);
	void         clearEnveloppes();
	QString      dialogStyleSheet();
};
