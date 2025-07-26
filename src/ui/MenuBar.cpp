#include "MenuBar.hpp"
#include "MenuBarButton.hpp"

#include <QHBoxLayout>
#include <QStringList>
#include <QSizePolicy>
#include <QPushButton>

MenuBar::MenuBar(QWidget* parent) : QWidget(parent)
{
	setStyle();
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(10, 0, 10, 0);
	addButtons(layout);
}

void	MenuBar::setStyle()
{
	setAttribute(Qt::WA_StyledBackground, true);
	setFixedHeight(50);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setStyleSheet
	(R"(
        background-color: #dbdbdbff;
        border-bottom: 1px solid #1a1a1a;
    )");
}

void	MenuBar::addButtons(QHBoxLayout* layout)
{
	QStringList labels =
	{
		"Enveloppes",
		"Historique\n履歴",
		"Statistiques\n統計",
		"Importer des dépenses\n支出をインポートする",
		"Transférer entre enveloppes\n封筒間で移動",
		"Ajouter une enveloppe\n封筒を追加する",
	};

	for(int i = 0; i < labels.size(); ++i)
	{
		MenuBarButton* btn = new MenuBarButton(labels[i], this);
		layout->addWidget(btn);
		connect(btn, &QPushButton::clicked, this, [this, i]()
		{
			emit menuButtonClicked(i);
		});
	}
}
