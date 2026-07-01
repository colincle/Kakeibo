#pragma once

#include "Envelope.hpp"
#include "EnvelopeManager.hpp"

#include <QList>
#include <QWidget>
#include <vector>

class KakeiboScrollArea;

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QProgressBar;

class Dispatch : public QWidget
{
	Q_OBJECT

  signals:
	void updateNeeded();

  public:
	Dispatch(QWidget *parent = nullptr);
	void showDispatch();

  private:
	struct DispatchRow
	{
		QWidget      *rowWidget;
		QLabel       *amountLabel;
		QProgressBar *progressBar;
		QLabel       *percentLabel;
		Envelope     *env;
		QPushButton  *lockButton;
	};

	struct Operation
	{
		int       amountAdded;
		Envelope *env;
		bool      lockSwitch;
	};

	std::vector<std::vector<Operation>> undoStack;
	std::vector<std::vector<Operation>> redoStack;

	QVBoxLayout       *layout        = nullptr;
	KakeiboScrollArea *scrollArea    = nullptr;
	QWidget           *scrollContent = nullptr;
	EnvelopeManager    dispatchManagerCopy;
	QList<DispatchRow> dispatchRows;
	DispatchRow        incomeRow;

	void appendIncomeRow(Envelope &env);
	void addIncomeRowButtons(QHBoxLayout *rowLayout);
	void connectIncomeRowButtons(QPushButton *undoBtn, QPushButton *redoBtn, QPushButton *applyBtn, QPushButton *dispatchBtn);

	void         appendRow(Envelope &env, EnvelopeManager &allEnvs);
	void         addNameLabel(QHBoxLayout *rowLayout, const Envelope &env);
	QPushButton *addButtons(QHBoxLayout *rowLayout, Envelope *env, EnvelopeManager &allEnvs);
	void         connectButtons(QPushButton *fillBtn, QPushButton *plusBtn, QPushButton *minusBtn, QPushButton *lockBtn, Envelope *env, EnvelopeManager &allEnvs);

	void updateRow(const DispatchRow &row);
	void updateAllRows();

	void fill(Envelope *env, EnvelopeManager &allEnvs);
	void incrementDispatch(Envelope *env, EnvelopeManager &allEnvs);
	void decrementDispatch(Envelope *env, EnvelopeManager &allEnvs);
	void dispatchIncomeEvenly();

	void apply();

	void undo();
	void redo();

	QString fillButtonStyle();
};
