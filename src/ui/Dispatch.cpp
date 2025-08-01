#include "Dispatch.hpp"
#include "Globals.hpp"
#include "Assets.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>

Dispatch::Dispatch(QWidget *parent)
    : QWidget(parent)
{
	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scrollArea->setStyleSheet(scrollAreaStyle());
	scrollArea->setFrameShape(QFrame::NoFrame);

	scrollContent = new QWidget;
	scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout = new QVBoxLayout(scrollContent);
	layout->setContentsMargins(0, 0, 0, 0);
	scrollContent->setLayout(layout);

	scrollArea->setWidget(scrollContent);

	auto *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(scrollArea);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	showDispatch();
}

void Dispatch::showDispatch()
{
	if ( !scrollArea || !layout )
		return;

	int savedScroll = scrollArea->verticalScrollBar()->value();

	for ( auto *row : enveloppeRows )
	{
		layout->removeWidget(row);
		delete row;
	}

	enveloppeRows.clear();
	appendIncomeRow(g_enveloppeManager.getIncomeEnveloppe());

	for ( auto &env : g_enveloppeManager.getEnveloppes() )
		appendRow(env);

	scrollArea->verticalScrollBar()->setValue(savedScroll);
}

void Dispatch::appendIncomeRow(Enveloppe &env)
{
	auto *rowWidget = new QWidget;
	auto *rowLayout = new QHBoxLayout(rowWidget);

	addNameLabel(rowLayout, env);
	addAmountLabel(rowLayout, env.getAmount());

	layout->addWidget(rowWidget);
	enveloppeRows.append(rowWidget);
}

void Dispatch::appendRow(Enveloppe &env)
{
	auto *rowWidget = new QWidget;
	auto *rowLayout = new QHBoxLayout(rowWidget);

	addNameLabel(rowLayout, env);
	addAmountLabel(rowLayout, env.getAmount());
	addProgressBar(rowLayout, env.getAmount(), env.getGoal());
	addButtons(rowLayout);

	layout->addWidget(rowWidget);
	enveloppeRows.append(rowWidget);
}

void Dispatch::addNameLabel(QHBoxLayout *rowLayout, const Enveloppe &env)
{
	auto *nameLabel = new QLabel(QString::fromStdString(env.getName()));
	nameLabel->setMinimumWidth(130);
	nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	rowLayout->addWidget(nameLabel);
}

void Dispatch::addAmountLabel(QHBoxLayout *rowLayout, int amount)
{
	QLocale jp(QLocale::Japanese, QLocale::Japan);
	QString formattedYen = jp.toString(amount);

	QString formattedEur;
	float   rate = getEurJpyRateCached();

	if ( rate > 0 )
	{
		QLocale fr(QLocale::French, QLocale::France);
		formattedEur = fr.toString(amount / rate, 'f', 2) + "€";
	}
	else
		formattedEur = "-€";

	auto *amountLabel = new QLabel(QString("¥%1\n%2").arg(formattedYen, formattedEur));
	amountLabel->setMinimumWidth(70);
	amountLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	rowLayout->addWidget(amountLabel);
}

void Dispatch::addProgressBar(QHBoxLayout *rowLayout, int amount, int goal)
{
	auto *bar = new QProgressBar;
	bar->setMinimumWidth(150);
	bar->setMaximum(goal);
	bar->setValue(amount);
	bar->setTextVisible(false);
	rowLayout->addWidget(bar);

	int percent = (goal > 0) ? (amount * 100 / goal) : 0;
	auto *percentLabel = new QLabel(QString("%1%").arg(percent));
	percentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	percentLabel->setMinimumWidth(40);
	rowLayout->addWidget(percentLabel);
}

void Dispatch::addButtons(QHBoxLayout *rowLayout)
{
	QStringList labels = { "fill to goal", "fill to max" };

	for (const auto &label : labels)
	{
		auto *btn = new QPushButton(label);
		btn->setStyleSheet(fillButtonStyle());
		rowLayout->addWidget(btn);
	}

	auto *plusBtn = new QPushButton;
	plusBtn->setIcon(QIcon(INCREMENT_ICON));
	plusBtn->setStyleSheet(iconButtonStyle());
	rowLayout->addWidget(plusBtn);

	auto *minusBtn = new QPushButton;
	minusBtn->setIcon(QIcon(DECREMENT_ICON));
	minusBtn->setStyleSheet(iconButtonStyle());
	rowLayout->addWidget(minusBtn);

	auto *lockBtn = new QPushButton;
	lockBtn->setIcon(QIcon(UNLOCKED_ICON));
	lockBtn->setStyleSheet(iconButtonStyle());
	lockBtn->setProperty("locked", false);

	connect(lockBtn, &QPushButton::clicked, this, [lockBtn]() {
		bool locked = lockBtn->property("locked").toBool();
		locked = !locked;
		lockBtn->setProperty("locked", locked);
		lockBtn->setIcon(QIcon(locked ? LOCKED_ICON : UNLOCKED_ICON));
	});

	rowLayout->addWidget(lockBtn);
}

QString Dispatch::iconButtonStyle()
{
	return R"(
		QPushButton {
			background-color: transparent;
			border: none;
			padding: 4px;
		}
		QPushButton:pressed {
			background-color: #1B272A;
			border-radius: 6px;
		}
	)";
}

QString Dispatch::fillButtonStyle()
{
	return R"(
		QPushButton {
			background-color: #1B272A;
			color: #E1E1E2;
			border: none;
			padding: 6px 12px;
			border-radius: 4px;
			min-width: 80px;
			font-family: "Helvetica Neue";
		}
		QPushButton:hover {
			background-color: #2F3D41;
		}
		QPushButton:pressed {
			background-color: #1B272A;
		}
	)";
}

QString Dispatch::scrollAreaStyle()
{
	return R"(
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
	       )";
}

//remove undo