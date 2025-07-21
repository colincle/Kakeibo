#include "MenuBar.hpp"
#include "MenuBarButton.hpp"
#include <QHBoxLayout>

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
		"Importer des dépenses\n支出をインポートする",
		"Transférer entre enveloppes\n封筒間で移動",
		"Ajouter une enveloppe\n封筒を追加する",
		"Historique\n履歴",
		"Statistiques\n統計"
	};

	for(const QString& label : labels)
		layout->addWidget(new MenuBarButton(label, this));
}