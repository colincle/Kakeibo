#pragma once
#include <QPushButton>

class MenuBarButton : public QPushButton
{
	Q_OBJECT

public:
	MenuBarButton(const QString& text, QWidget* parent = nullptr);
};
