#include "MenuBar.hpp"
#include "MenuBarButton.hpp"
#include "Globals.hpp"
#include "Assets.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QSize>
#include <QSizePolicy>
#include <QStyle>
#include <QLocale>

MenuBar::MenuBar(QWidget* parent) : QWidget(parent)
{
	setStyle();
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(10, 0, 10, 0);
	layout->setSpacing(0);

	addPagesButtons(layout);
	createTotalLabel(layout);
	layout->addSpacing(5);
	addActionsButtons(layout);
}

void MenuBar::createTotalLabel(QHBoxLayout* layout)
{
	if(totalLabel)
	{
		layout->removeWidget(totalLabel);
		delete totalLabel;
		totalLabel = nullptr;
	}

	int total = 0;

	for(const auto& e : g_enveloppeManager.getEnveloppes())
		total += e.getAmount();

	QString formatted = QLocale(QLocale::Japanese, QLocale::Japan).toString(total);

	totalLabel = new QLabel(QString("¥%1").arg(formatted), this);
	totalLabel->setStyleSheet(R"(
		color: #E1E1E2;
		font-family: 'Helvetica Neue';
		font-size: 18px;
	)");
	totalLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	totalLabel->setMinimumWidth(120);

	layout->addWidget(totalLabel);
}

void	MenuBar::setStyle()
{
	setAttribute(Qt::WA_StyledBackground, true);
	setFixedHeight(55);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setStyleSheet
	(R"(
        background-color: #1B272A;
    )");
}

void MenuBar::addActionsButtons(QHBoxLayout* layout)
{
	QStringList icons = { IMPORT_ICON, TRANSFER_ICON, PLUS_ICON };

	for(int i = 0; i < icons.size(); ++i)
	{
		QPushButton* btn = new QPushButton(this);
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
		{
			emit menuButtonClicked(index);
		});
	}
}

void MenuBar::addPagesButtons(QHBoxLayout* layout)
{
	QStringList labels =
	{
		"Enveloppes\n封筒",
		"Historique\n履歴",
		"Statistiques\n統計",
	};

	for(int i = 0; i < labels.size(); ++i)
	{
		MenuBarButton* btn = new MenuBarButton(labels[i], this);
		btn->setProperty("index", i);

		btn->setFixedHeight(this->height());
		layout->setAlignment(Qt::AlignTop);
		layout->addWidget(btn);
		connect(btn, &QPushButton::clicked, this, [this, i]()
		{
			setActiveButton(i);
			emit menuButtonClicked(i);
		});
		buttons.append(btn);
	}

	setActiveButton(0);
}

void MenuBar::setActiveButton(int index)
{
	for(int i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->setProperty("active", i == index);
		buttons[i]->style()->unpolish(buttons[i]);
		buttons[i]->style()->polish(buttons[i]);
		buttons[i]->update();
	}
}
