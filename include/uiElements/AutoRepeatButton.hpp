#pragma once

#include <QPushButton>
#include <QIcon>

class AutoRepeatButton : public QPushButton
{
	Q_OBJECT

  public:
	explicit AutoRepeatButton(const QIcon &icon, QWidget *parent = nullptr)
	    : QPushButton(parent)
	{
		setIcon(icon);
		setAutoRepeat(true);
		setAutoRepeatDelay(300);
		setAutoRepeatInterval(60);
		setStyleSheet(style());
	}

	static QString style()
	{
		return R"(
			QPushButton {
				background-color: transparent;
				border: none;
				padding: 4px;
			}
			QPushButton:pressed {
				background-color: #1B272A;
				border-radius: 6px;
			}
		)";
	}
};
