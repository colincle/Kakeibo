#pragma once
#include <QWidget>

class QHBoxLayout;

class MenuBar : public QWidget
{
	Q_OBJECT

signals:
	void menuButtonClicked(int index);

public:
	explicit MenuBar(QWidget* parent = nullptr);

private:
	void	setStyle();
	void	addButtons(QHBoxLayout* layout);
};
