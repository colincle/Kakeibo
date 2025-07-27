#pragma once

#include <QWidget>
#include <QVector>

class QHBoxLayout;
class QLabel;
class QPushButton;

class MenuBar : public QWidget
{
	Q_OBJECT

public:
	explicit MenuBar(QWidget* parent = nullptr);

signals:
	void menuButtonClicked(int index);

private:
	QVector<QPushButton*> buttons;
	QLabel* totalLabel = nullptr;

	void setStyle();
	void addPagesButtons(QHBoxLayout* layout);
	void addActionsButtons(QHBoxLayout* layout);
	void setActiveButton(int index);
	void createTotalLabel(QHBoxLayout* layout);
};
