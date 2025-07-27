#pragma once

#include <QVector>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QPushButton;

class MenuBar : public QWidget
{
	Q_OBJECT
  signals:
	void menuButtonClicked(int index);

  public:
	explicit MenuBar(QWidget *parent = nullptr);

	void updateTotalLabel();

  private:
	QVector<QPushButton *> buttons;
	QLabel                *totalLabel = nullptr;

	void setStyle();
	void addPagesButtons(QHBoxLayout *layout);
	void addActionsButtons(QHBoxLayout *layout);
	void setActiveButton(int index);
	void createTotalLabel(QHBoxLayout *layout);
};
