#pragma once

#include <QHeaderView>
#include <QTableWidget>

class KakeiboTable : public QTableWidget
{
	Q_OBJECT

  public:
	explicit KakeiboTable(QWidget *parent = nullptr)
	    : QTableWidget(parent)
	{
		horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		horizontalHeader()->setStretchLastSection(true);
		setTextElideMode(Qt::ElideNone);
		setWordWrap(false);
		setEditTriggers(QAbstractItemView::NoEditTriggers);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::SingleSelection);
		verticalHeader()->setVisible(false);
		horizontalHeader()->setHighlightSections(false);
		setShowGrid(true);
		setGridStyle(Qt::SolidLine);
		setStyleSheet(R"(
			QTableWidget {
				background-color: #1B272A;
				color: #E1E1E2;
				selection-background-color: #2F3D41;
				border-left: 1px solid #444;
				border-top: 1px solid #444;
				font-family: "Helvetica Neue";
				gridline-color: grey;
				margin-top: 8px;
			}
			QHeaderView::section {
				background-color: #242F32;
				color: #E1E1E2;
				padding: 4px;
				border: none;
			}
			QScrollBar:vertical {
				background: transparent;
				width: 8px;
				margin: 3px 0;
				border-radius: 4px;
				padding-top: 28px;
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
};
