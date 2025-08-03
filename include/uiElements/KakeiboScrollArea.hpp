#pragma once

#include <QScrollArea>
#include <QScrollBar>

class KakeiboScrollArea : public QScrollArea
{
	Q_OBJECT

  public:
	explicit KakeiboScrollArea(QWidget *parent = nullptr)
	    : QScrollArea(parent)
	{
		setWidgetResizable(true);
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setFrameShape(QFrame::NoFrame);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setStyleSheet(R"(
			QScrollBar:vertical {
				background: transparent;
				width: 8px;
				margin: 3px 0;
				border-radius: 4px;
			}
			QScrollBar::handle:vertical {
				background: #1B272A;
				min-height: 20px;
				border-radius: 4px;
			}
			QScrollBar::add-line:vertical,
			QScrollBar::sub-line:vertical {
				height: 0;
			}
			QScrollBar::add-page:vertical,
			QScrollBar::sub-page:vertical {
				background: none;
			}
		)");
	}

	void saveScroll()
	{
		savedScroll = verticalScrollBar()->value();
	}

	void restoreScroll()
	{
		QMetaObject::invokeMethod(this, [this]()
		                          { verticalScrollBar()->setValue(savedScroll); }, Qt::QueuedConnection);
	}

  private:
	int savedScroll = 0;
};
