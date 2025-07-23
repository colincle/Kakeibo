#pragma once

#include <QWidget>

class QGridLayout;
class QStackedWidget;
class EnveloppesUi;
class MenuBar;

class MainView : public QWidget
{
	Q_OBJECT

public:
	explicit MainView(MenuBar* menuBar, QWidget* parent = nullptr);

private:
	QStackedWidget* stack = nullptr;
	EnveloppesUi* enveloppesUI = nullptr;

	void handleMenuAction(int i);
};
