#include "MenuBar.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

MenuBar::MenuBar(QWidget* parent) : QWidget(parent)
{
	setAttribute(Qt::WA_StyledBackground, true);
	setFixedHeight(50);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setStyleSheet
	(R"(
        background-color: #dbdbdbff;
        border-bottom: 1px solid #1a1a1a;
    )");

	QHBoxLayout* layout = new QHBoxLayout(this);

	layout->setContentsMargins(10, 0, 10, 0);

	QStringList labels =
	{
		"Importer des dépenses\n支出をインポートする",
		"Transférer entre enveloppes\n封筒間で移動",
		"Ajouter une enveloppe\n封筒を追加する",
		"Historique\n履歴",
		"Statistiques\n統計"
	};

	for(const QString& label : labels)
	{
		QPushButton* btn = new QPushButton(label, this);
		btn->setFlat(true);
		btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		btn->setFixedHeight(40);

		btn->setStyleSheet
		(R"(
            font-family: "Helvetica Neue";
            font-weight: "100";
            background-color: #cccccc;
            border: #dddddd;
            border: 1px solid;
            border-radius: 5px;
            color: black;
            padding: 4px 10px;
        )");

		layout->addWidget(btn);
	}
}
