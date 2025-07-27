#pragma once

#include <QWidget>

class QGridLayout;
class QStackedWidget;
class EnveloppesUi;
class History;
class Stats;
class MenuBar;

class MainView : public QWidget
{
	Q_OBJECT

  public:
	explicit MainView(MenuBar *menuBar, QWidget *parent = nullptr);

  private:
	QStackedWidget *stack        = nullptr;
	EnveloppesUi   *enveloppesUI = nullptr;
	History        *history      = nullptr;
	Stats          *stats        = nullptr;
	MenuBar        &menuBar;

	void handleMenuAction(int i);
	void updatePages();
};
