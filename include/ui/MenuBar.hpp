#pragma once

#include <QVector>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QPushButton;

// Identifies which menu entry was clicked. The first MENU_PAGE_COUNT values are
// the page tabs (in stack order); the rest are one-shot actions.
enum class MenuAction
{
	Envelopes,
	History,
	Stats,
	Dispatch,
	Import,
	Transfer,
	AddEnvelope
};

inline constexpr int MENU_PAGE_COUNT = 4;

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
