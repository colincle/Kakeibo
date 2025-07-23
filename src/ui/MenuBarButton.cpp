#include "MenuBarButton.hpp"

#include <QSizePolicy>
#include <QString>

MenuBarButton::MenuBarButton(const QString& text, QWidget* parent)
	: QPushButton(text, parent)
{
	setFlat(true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	setFixedHeight(40);

	setStyleSheet
	(R"(
		font-family: "Helvetica Neue";
		font-weight: 100;
		background-color: #cccccc;
		border: #dddddd;
		border: 1px solid;
		border-radius: 5px;
		color: black;
		padding: 4px 10px;
	)");
}