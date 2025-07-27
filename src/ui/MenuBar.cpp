#include "MenuBar.hpp"
#include "Assets.hpp"
#include "Globals.hpp"
#include "MenuBarButton.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>
#include <QStyle>

MenuBar::MenuBar(QWidget *parent) : QWidget(parent)
{
	setStyle();
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(10, 0, 10, 0);
	layout->setSpacing(0);

	addPagesButtons(layout);
	createTotalLabel(layout);
	layout->addSpacing(5);
	addActionsButtons(layout);
}

void MenuBar::createTotalLabel(QHBoxLayout *layout)
{
	totalLabel = new QLabel(this);
	totalLabel->setStyleSheet(R"(
		color: #E1E1E2;
		font-family: 'Helvetica Neue';
		font-size: 18px;
	)");
	totalLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	totalLabel->setMinimumWidth(120);

	layout->addWidget(totalLabel);
	updateTotalLabel();
}

void MenuBar::updateTotalLabel()
{
	if ( !totalLabel )
		return;

	int total = 0;

	for ( const auto &e : g_enveloppeManager.getEnveloppes() )
		total += e.getAmount();

	QLocale jp(QLocale::Japanese, QLocale::Japan);
	QString formattedYen = jp.toString(total);

	QString formattedEur;
	float   rate = getEurJpyRateCached();

	if ( rate > 0 )
	{
		QLocale fr(QLocale::French, QLocale::France);
		formattedEur = fr.toString(total / rate, 'f', 2) + "€";
	}
	else
		formattedEur = "-€";

	totalLabel->setText(QString("¥%1 | %2").arg(formattedYen, formattedEur));
}

void MenuBar::setStyle()
{
	setAttribute(Qt::WA_StyledBackground, true);
	setFixedHeight(55);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setStyleSheet(R"(
        background-color: #1B272A;
    )");
}

void MenuBar::addActionsButtons(QHBoxLayout *layout)
{
	QStringList icons = {IMPORT_ICON, TRANSFER_ICON, PLUS_ICON};

	for ( int i = 0; i < icons.size(); ++i )
	{
		QPushButton *btn = new QPushButton(this);
		btn->setStyleSheet(R"(
			QPushButton {
				background-color: #1B272A;
				border-radius: 20px;
			}
			QPushButton:pressed {
				background-color: #2F3D41;
			}
		)");
		btn->setIcon(QIcon(icons[i]));
		btn->setFlat(true);
		btn->setCursor(Qt::PointingHandCursor);
		btn->setFixedSize(40, 40);
		btn->setIconSize(QSize(35, 35));
		layout->addWidget(btn);
		buttons.append(btn);

		int index = i + 3;
		connect(btn, &QPushButton::clicked, this, [this, index]()
		        { emit menuButtonClicked(index); });
	}
}

void MenuBar::addPagesButtons(QHBoxLayout *layout)
{
	QStringList labels =
	    {
	        "Enveloppes\n封筒",
	        "Historique\n履歴",
	        "Statistiques\n統計",
	    };

	for ( int i = 0; i < labels.size(); ++i )
	{
		MenuBarButton *btn = new MenuBarButton(labels[i], this);
		btn->setProperty("index", i);

		btn->setFixedHeight(this->height());
		layout->setAlignment(Qt::AlignTop);
		layout->addWidget(btn);
		connect(btn, &QPushButton::clicked, this, [this, i]()
		        {
			setActiveButton(i);
			emit menuButtonClicked(i); });
		buttons.append(btn);
	}

	setActiveButton(0);
}

void MenuBar::setActiveButton(int index)
{
	for ( int i = 0; i < buttons.size(); ++i )
	{
		buttons[i]->setProperty("active", i == index);
		buttons[i]->style()->unpolish(buttons[i]);
		buttons[i]->style()->polish(buttons[i]);
		buttons[i]->update();
	}
}
