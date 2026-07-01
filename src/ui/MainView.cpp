#include "MainView.hpp"
#include "Backups.hpp"
#include "ConnectToCloud.hpp"
#include "Dispatch.hpp"
#include "EnvelopesTransfer.hpp"
#include "EnvelopesUi.hpp"
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

	Backups::backup();

	stack = new QStackedWidget(this);
	layout->addWidget(stack);

	envelopesUI = new EnvelopesUi(this);
	stack->addWidget(envelopesUI);

	history = new History(this);
	stack->addWidget(history);

	stats = new Stats(this);
	stack->addWidget(stats);

	dispatch = new Dispatch(this);
	stack->addWidget(dispatch);

	stack->setCurrentIndex(0);

	connect(menuBar, &MenuBar::menuButtonClicked, this, &MainView::handleMenuAction);
	connect(envelopesUI, &EnvelopesUi::updateNeeded, this, &MainView::updatePages);
	connect(history, &History::updateNeeded, this, &MainView::updatePages);
	connect(dispatch, &Dispatch::updateNeeded, this, &MainView::updatePages);
}

void MainView::handleMenuAction(int index)
{
	switch ( static_cast<MenuAction>(index) )
	{
	case MenuAction::Envelopes:
		stack->setCurrentWidget(envelopesUI);
		break;

	case MenuAction::History:
		stack->setCurrentWidget(history);
		break;

	case MenuAction::Stats:
		stack->setCurrentWidget(stats);
		break;

	case MenuAction::Dispatch:
		stack->setCurrentWidget(dispatch);
		break;

	case MenuAction::Import:
		ImportExpenses::import(this);
		updatePages();
		break;

	case MenuAction::Transfer:
		EnvelopesTransfer::transfer(this);
		updatePages();
		break;

	case MenuAction::AddEnvelope:
		envelopesUI->addEnvelope("");
		updatePages();
		break;
	}
}

void MainView::updatePages()
{
	envelopesUI->showEnvelopes();
	history->showHistory();
	stats->showStats();
	dispatch->showDispatch();
	menuBar.updateTotalLabel();
	ConnectToCloud::sendCardsToCloud(envelopesUI->getCloudCardWidgets());
}