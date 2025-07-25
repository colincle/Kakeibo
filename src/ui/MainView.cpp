#include "MainView.hpp"
#include "EnveloppesTransfer.hpp"
#include "MenuBar.hpp"
#include "EnveloppesUi.hpp"
#include "ImportExpenses.hpp"

#include <QVBoxLayout>
#include <QStackedWidget>

MainView::MainView(MenuBar* menuBar, QWidget* parent) : QWidget(parent)
{
	connect(menuBar, &MenuBar::menuButtonClicked, this, &MainView::handleMenuAction);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 0, 10, 0);

	stack = new QStackedWidget(this);
	layout->addWidget(stack);

	enveloppesUI = new EnveloppesUi(this);
	stack->addWidget(enveloppesUI); // index 0

	// Add other pages here, e.g.
	// summaryUI = new SummaryUI(this);
	// stack->addWidget(summaryUI); // index 1

	stack->setCurrentIndex(0);

}

void MainView::handleMenuAction(int index)
{
	switch(index)
	{
	case 0:
		ImportExpenses::import(this);
		enveloppesUI->showEnveloppes();
		break;

	case 1:
		EnveloppesTransfer::transfer(this);
		enveloppesUI->showEnveloppes();
		break;

	case 2:
		enveloppesUI->addEnveloppe();
		break;

	case 3:
		// Handle "Historique"
		break;

	case 4:
		// Handle "Statistiques"
		break;

	default:
		break;
	}
}
