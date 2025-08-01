#pragma once //WIP

#include "Enveloppe.hpp"
#include <QList>
#include <QVBoxLayout>
#include <QWidget>

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

	void appendRow(Enveloppe &env);
	void appendIncomeRow(Enveloppe &env);
	void addNameLabel(QHBoxLayout *rowLayout, const Enveloppe &env);
	void addAmountLabel(QHBoxLayout *rowLayout, int amount);
	void addProgressBar(QHBoxLayout *rowLayout, int amount, int goal);
	void addButtons(QHBoxLayout *rowLayout);
	QString iconButtonStyle();
	QString fillButtonStyle();
	QString scrollAreaStyle();

	QVBoxLayout     *layout        = nullptr;
	QScrollArea     *scrollArea    = nullptr;
	QWidget         *scrollContent = nullptr;
	QList<QWidget *> enveloppeRows;
};
