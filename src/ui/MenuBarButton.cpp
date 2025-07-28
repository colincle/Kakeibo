#include "MenuBarButton.hpp"

#include <QSizePolicy>
#include <QString>

MenuBarButton::MenuBarButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
	setProperty("active", false);
	setFlat(true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	setFixedHeight(50);
	setStyleSheet(R"(
		QPushButton {
			font-family: 'Helvetica Neue';
			font-size: 16px;
			color: #E1E1E2;
			background-color: #1B272A;
			border-bottom: 1px solid #2F3D41;
		}
		QPushButton[active="true"] {
			color: #337BFF;
			border: none;
			background-color: #242F32;
			border-top-left-radius: 16px;
			border-top-right-radius: 16px;
		}
	)");
}
