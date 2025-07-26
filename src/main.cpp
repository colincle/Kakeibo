#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>
#include <QSizePolicy>

#include "MenuBar.hpp"
#include "MainView.hpp"

QWidget* createMainWindow()
{
	QWidget* window = new QWidget;
	window->setAttribute(Qt::WA_StyledBackground, true);
	window->setAutoFillBackground(true);
	window->setStyleSheet("background-color: white;");
	window->setWindowTitle("Kakeibo");
	window->resize(1280, 720);
	window->setMinimumSize(840, 420);
	return window;
}

void	setUpShortcuts(QWidget* window)
{
	QShortcut* closeShortcut = new QShortcut(QKeySequence("Ctrl+W"), window);
	QObject::connect(closeShortcut, &QShortcut::activated, window, &QWidget::close);
}

void	setUpAppLayout(QWidget* window)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(window);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	MenuBar* menuBar = new MenuBar(window);
	MainView* mainView = new MainView(menuBar, window);
	mainView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainLayout->addWidget(menuBar);
	mainLayout->addWidget(mainView, 1);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QWidget* window = createMainWindow();
	setUpShortcuts(window);
	setUpAppLayout(window);
	window->show();
	return app.exec();
}

/*
TO DO

- Format style sheets
- Make the tables non writtable
*/