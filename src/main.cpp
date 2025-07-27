#include <QApplication>
#include <QKeySequence>
#include <QShortcut>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

#include "MainView.hpp"
#include "MenuBar.hpp"

QWidget *createMainWindow()
{
	QWidget *window = new QWidget;
	window->setAttribute(Qt::WA_StyledBackground, true);
	window->setAutoFillBackground(true);
	window->setStyleSheet("background-color: #242F32;");
	window->setWindowTitle("Kakeibo");
	window->resize(1110, 850);
	window->setMinimumSize(840, 420);
	return window;
}

void setUpShortcuts(QWidget *window)
{
	QShortcut *closeShortcut = new QShortcut(QKeySequence("Ctrl+W"), window);
	QObject::connect(closeShortcut, &QShortcut::activated, window, &QWidget::close);
}

void setUpAppLayout(QWidget *window)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(window);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	MenuBar  *menuBar  = new MenuBar(window);
	MainView *mainView = new MainView(menuBar, window);
	mainView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainLayout->addWidget(menuBar);
	mainLayout->addSpacing(8);
	mainLayout->addWidget(mainView, 1);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QWidget     *window = createMainWindow();
	setUpShortcuts(window);
	setUpAppLayout(window);
	window->show();
	return app.exec();
}
