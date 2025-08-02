#pragma once //WIP

#include "Enveloppe.hpp"
#include "EnveloppeManager.hpp"
#include <QList>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

class QLabel;
class QPushButton;
class QProgressBar;
class QScrollArea;
class QScrollBar;

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
		Enveloppe    *env;
		QPushButton  *lockButton;
	};

	struct Operation
	{
		int        amountAdded;
		Enveloppe *env;
		bool       lockSwitch;
	};

	std::vector<std::vector<Operation>> undoStack;
	std::vector<std::vector<Operation>> redoStack;

	QVBoxLayout       *layout        = nullptr;
	QScrollArea       *scrollArea    = nullptr;
	QWidget           *scrollContent = nullptr;
	EnveloppeManager   dispatchManagerCopy;
	QList<DispatchRow> dispatchRows;
	DispatchRow        incomeRow;

	void         appendRow(Enveloppe &env, EnveloppeManager &allEnvs);
	void         appendIncomeRow(Enveloppe &env);
	void         addNameLabel(QHBoxLayout *rowLayout, const Enveloppe &env);
	void         addAmountLabel(QHBoxLayout *rowLayout, int amount);
	void         addProgressBar(QHBoxLayout *rowLayout, int amount, int goal);
	QPushButton *addButtons(QHBoxLayout *rowLayout, Enveloppe *env, EnveloppeManager &allEnvs);
	void         connectButtons(QPushButton *fillBtn, QPushButton *plusBtn, QPushButton *minusBtn, QPushButton *lockBtn, Enveloppe *env, EnveloppeManager &allEnvs);
	void         fill(Enveloppe *env, EnveloppeManager &allEnvs);
	void         incrementDispatch(Enveloppe *env, EnveloppeManager &allEnvs);
	void         decrementDispatch(Enveloppe *env, EnveloppeManager &allEnvs);
	void         dispatchIncomeEvenly();
	void         apply();
	void         undo();
	void         redo();
	QString      iconButtonStyle();
	QString      fillButtonStyle();
	QString      scrollAreaStyle();

	void updateRow(const DispatchRow &row);
	void updateAllRows();
};
