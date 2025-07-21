#include "MainView.hpp"
#include <QHBoxLayout>
#include <QLabel>

MainView::MainView(QWidget* parent) : QWidget(parent)
{
	setStyleSheet("background-color: #f5f5f5;");

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(10, 0, 10, 0);
	layout->addWidget(new QLabel(" "));
}
