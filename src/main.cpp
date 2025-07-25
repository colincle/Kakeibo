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

// #include <iostream>
// #include <vector>
// #include <string>
// #include <chrono>
// #include "Parser.hpp"

// int main()
// {

// std::string input =
//	"7/3\t977\t0\tデビット1 292747 イオングル－フ\n"
//	"7/4\t0\t165842\t振込IB1 コラン ナツキ\n"
//	"7/4\t0\t167300\t振込1 REVOLUT TECHNOL\n"
//	"7/4\t570\t0\tデビット1 893631 トモウイメンズ";	std::chrono::year year{2025};
//	std::vector<Expense> expenses = Parser::parseExpenses(input, year);

//	for (const auto& e : expenses)
//	{
//	std::cout << "Date: |"
// << int(e.date.year()) << '-'
// << unsigned(e.date.month()) << '-'
// << unsigned(e.date.day()) << "|\n"
// << "Debit: |"  << e.debit  << "|\n"
// << "Credit: |" << e.credit << "|\n"
// << "Info: |"   << e.info   << "|\n\n";
//	}

//	return 0;
// }
