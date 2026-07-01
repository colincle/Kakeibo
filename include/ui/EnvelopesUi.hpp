#pragma once

#include "Envelope.hpp"

#include <QList>
#include <QString>
#include <QWidget>

class KakeiboScrollArea;

class QCheckBox;
class QDialog;
class QFormLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;
class QVBoxLayout;
class QHBoxLayout;

class EnvelopesUi : public QWidget
{
	Q_OBJECT

  signals:
	void updateNeeded();

  public:
	explicit EnvelopesUi(QWidget *parent = nullptr);
	void             showEnvelopes();
	void             addEnvelope(std::string name);
	QList<QWidget *> getCloudCardWidgets() { return cloudCardWidgets; }

  protected:
	void resizeEvent(QResizeEvent *event) override;

  private:
	struct EnvelopeFormFields
	{
		QLineEdit *nameFrInput    = nullptr;
		QLineEdit *nameJpInput    = nullptr;
		QLineEdit *nameInput      = nullptr;
		QLineEdit *amountInput    = nullptr;
		QLineEdit *maxAmountInput = nullptr;
		QLineEdit *goalInput      = nullptr;
		QCheckBox *savingsCheck   = nullptr;
	};

	QGridLayout       *gridLayout          = nullptr;
	QTimer            *resizeDebounceTimer = nullptr;
	KakeiboScrollArea *scrollArea          = nullptr;
	QWidget           *scrollContent       = nullptr;
	QList<QWidget *>   cloudCardWidgets;
	QWidget           *topInfoWidget = nullptr;
	QHBoxLayout       *topInfoLayout = nullptr;

	void clearGrid();
	int  computeColumnCount() const;
	void populateGrid(int columnCount);

	void                 showTopInfo();
	QWidget             *createCard(const Envelope &env);
	QVBoxLayout         *setupCardLayout(QWidget *card);
	void                 addCardContent(QVBoxLayout *layout, const Envelope &env, const QString &barColor, int percent);
	QWidget             *createCardButtons(const Envelope &env);
	QList<QPushButton *> createCardButtonList(const Envelope &env);
	void                 connectCardButtons(const Envelope &env, const QList<QPushButton *> &buttons);
	QWidget             *createProgressLabel(QString barColor, int percent);
	QWidget             *createGoalMaxLabel(const Envelope &env);
	QString              getProgressBarColor(int percent, const Envelope &env);

	void deleteEnvelope(const Envelope &env);
	void showNonEmptyWarning();
	bool confirmDeletion();
	void addDialogButtons(QDialog *dialog, QFormLayout *layout);
	void handleEnvelopeSubmission(const EnvelopeFormFields &f, std::string name);
	void createFields(QDialog *dialog, QFormLayout *layout, EnvelopeFormFields &f);
	void setupEnvelopeDialog(QDialog &dialog);
	void preFillFieldsFromName(const std::string &name, EnvelopeFormFields &fields);

	KakeiboScrollArea *createScrollArea(QWidget *content);
	void               clearEnvelopes();
	QString            dialogStyleSheet();
};
