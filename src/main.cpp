#include <cstdlib>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QHBoxLayout>
#include <QtCore/QObject>
#include <QShortcut>
#include <QStyleFactory>
#include <MenuBar.hpp>
#include <MainView.hpp>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QWidget window;
	window.setAttribute(Qt::WA_StyledBackground, true);
	window.setAutoFillBackground(true);
	window.setStyleSheet("background-color: #f5f5f5;");
	window.setWindowTitle("Kakeibo");

	QShortcut* closeShortcut = new QShortcut(QKeySequence("Ctrl+W"), &window);
	QObject::connect(closeShortcut, &QShortcut::activated, &window, &QWidget::close);

	QVBoxLayout* mainLayout = new QVBoxLayout(&window);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	MenuBar* menuBar = new MenuBar(&window);
	MainView* mainView = new MainView(menuBar, &window);

	mainView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	mainLayout->addWidget(menuBar);
	mainLayout->addWidget(mainView, 1);

	window.resize(1280, 720);
	window.setMinimumSize(840, 420);
	window.show();
	return app.exec();
}
