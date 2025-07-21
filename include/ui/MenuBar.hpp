#pragma once
#include <QWidget>

class QHBoxLayout;

class MenuBar : public QWidget
{
	Q_OBJECT

public:
	explicit MenuBar(QWidget* parent = nullptr);

private:
	void	setStyle();
	void	addButtons(QHBoxLayout* layout);
};
