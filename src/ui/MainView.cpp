#include "MainView.hpp"
#include "ConnectToCloud.hpp"
#include "EnveloppesTransfer.hpp"
#include "EnveloppesUi.hpp"
#include "History.hpp"
#include "ImportExpenses.hpp"
#include "MenuBar.hpp"
#include "Stats.hpp"

#include <QStackedWidget>
#include <QVBoxLayout>

MainView::MainView(MenuBar *menuBar, QWidget *parent) : QWidget(parent), menuBar(*menuBar)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
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

	connect(menuBar, &MenuBar::menuButtonClicked, this, &MainView::handleMenuAction);
	connect(enveloppesUI, &EnveloppesUi::updateNeeded, this, &MainView::updatePages);
	connect(history, &History::updateNeeded, this, &MainView::updatePages);
}

void MainView::handleMenuAction(int index)
{
	switch ( index )
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
		enveloppesUI->addEnveloppe("");
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
	menuBar.updateTotalLabel();
	ConnectToCloud::sendCardsToCloud(enveloppesUI->getCloudCardWidgets());
}