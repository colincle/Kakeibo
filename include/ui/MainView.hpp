#pragma once
#include <QWidget>
#include <QGridLayout>

class MenuBar;

class MainView : public QWidget
{
	Q_OBJECT

public:
	explicit MainView(MenuBar* menuBar, QWidget* parent = nullptr);

private:
	QGridLayout* gridLayout = nullptr;

	void handleMenuAction(int i);
	void showEnveloppes();
	void addEnveloppe();
};
