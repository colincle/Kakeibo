#pragma once

#include <QPushButton>
#include <QIcon>

class IconButton : public QPushButton
{
	Q_OBJECT

  public:
	explicit IconButton(const QIcon &icon, QWidget *parent = nullptr)
	    : QPushButton(parent)
	{
		setIcon(icon);
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
