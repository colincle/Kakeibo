#include "MainView.hpp"
#include "EnveloppesTransfer.hpp"
#include "MenuBar.hpp"
#include "EnveloppesUi.hpp"
#include "History.hpp"
#include "Stats.hpp"
#include "ImportExpenses.hpp"

#include <QVBoxLayout>
#include <QStackedWidget>

MainView::MainView(MenuBar* menuBar, QWidget* parent) : QWidget(parent)
{
	connect(menuBar, &MenuBar::menuButtonClicked, this, &MainView::handleMenuAction);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(3, 0, 3, 0);

	stack = new QStackedWidget(this);
	layout->addWidget(stack);

	enveloppesUI = new EnveloppesUi(this);
	stack->addWidget(enveloppesUI);

	history = new History(this);
	stack->addWidget(history);

	stats = new Stats(this);
	stack->addWidget(stats);

	stack->setCurrentIndex(0);

}

void MainView::handleMenuAction(int index)
{
	switch(index)
	{
	case 0:
		stack->setCurrentWidget(enveloppesUI);
		break;

	case 1:
		stack->setCurrentWidget(history);
		break;

	case 2:
		stack->setCurrentWidget(stats);
		break;

	case 3:
		ImportExpenses::import(this);
		updatePages();
		break;

	case 4:
		EnveloppesTransfer::transfer(this);
		updatePages();
		break;

	case 5:
		enveloppesUI->addEnveloppe();
		updatePages();
		break;

	default:
		break;
	}
}

void MainView::updatePages()
{
	enveloppesUI->showEnveloppes();
	history->showHistory();
	stats->showStats();
}