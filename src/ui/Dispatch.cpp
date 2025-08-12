#include "Dispatch.hpp"
#include "Assets.hpp"
#include "AutoRepeatButton.hpp"
#include "Globals.hpp"
#include "IconButton.hpp"
#include "KakeiboScrollArea.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>

Dispatch::Dispatch(QWidget *parent)
    : QWidget(parent)
{
	scrollArea = new KakeiboScrollArea(this);

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

	scrollArea->saveScroll();

	for ( auto &row : dispatchRows )
	{
		layout->removeWidget(row.rowWidget);
		delete row.rowWidget;
	}

	dispatchRows.clear();
	undoStack.clear();

	dispatchManagerCopy = EnveloppeManager(g_enveloppeManager);
	appendIncomeRow(dispatchManagerCopy.getIncomeEnveloppe());

	for ( auto &env : dispatchManagerCopy.getEnveloppes() )
	{
		if ( !env.isSavings() )
		{
			int diff = env.getAmount();
			env.setAmount(0);
			dispatchManagerCopy.getIncomeEnveloppe().setAmount(dispatchManagerCopy.getIncomeEnveloppe().getAmount() + diff);
		}

		appendRow(env, dispatchManagerCopy);
	}

	updateAllRows();
	scrollArea->restoreScroll();
}

void Dispatch::appendIncomeRow(Enveloppe &env)
{
	if ( incomeRow.rowWidget )
	{
		layout->removeWidget(incomeRow.rowWidget);
		delete incomeRow.rowWidget;
	}

	auto *rowWidget = new QWidget;
	auto *rowLayout = new QHBoxLayout(rowWidget);

	addNameLabel(rowLayout, env);

	auto *amountLabel = new QLabel;
	rowLayout->addWidget(amountLabel);

	addIncomeRowButtons(rowLayout);

	layout->addWidget(rowWidget);

	incomeRow = {rowWidget, amountLabel, nullptr, nullptr, &env, nullptr};
	updateRow(incomeRow);
}

void Dispatch::addIncomeRowButtons(QHBoxLayout *rowLayout)
{
	auto *undoBtn = new AutoRepeatButton(QIcon(UNDO_ICON));
	auto *redoBtn = new AutoRepeatButton(QIcon(REDO_ICON));

	auto *applyBtn = new QPushButton("Répartir 分配");
	applyBtn->setStyleSheet(fillButtonStyle());
	applyBtn->setFixedHeight(28);
	applyBtn->setMinimumWidth(70);

	auto *dispatchBtn = new QPushButton("Appliquer 適用");
	dispatchBtn->setStyleSheet(fillButtonStyle());
	dispatchBtn->setFixedHeight(28);
	dispatchBtn->setMinimumWidth(70);

	auto *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(undoBtn);
	buttonsLayout->addWidget(redoBtn);
	buttonsLayout->addWidget(applyBtn);
	buttonsLayout->addWidget(dispatchBtn);
	rowLayout->addLayout(buttonsLayout);

	connectIncomeRowButtons(undoBtn, redoBtn, applyBtn, dispatchBtn);
}

void Dispatch::connectIncomeRowButtons(QPushButton *undoBtn, QPushButton *redoBtn, QPushButton *applyBtn, QPushButton *dispatchBtn)
{
	connect(undoBtn, &QPushButton::clicked, this, [this]()
	        { undo(); });
	connect(redoBtn, &QPushButton::clicked, this, [this]()
	        { redo(); });
	connect(applyBtn, &QPushButton::clicked, this, [this]()
	        { dispatchIncomeEvenly(); });
	connect(dispatchBtn, &QPushButton::clicked, this, [this]()
	        { apply(); });
}

void Dispatch::appendRow(Enveloppe &env, EnveloppeManager &allEnvs)
{
	auto *rowWidget = new QWidget;
	auto *rowLayout = new QHBoxLayout(rowWidget);

	addNameLabel(rowLayout, env);

	auto *amountLabel = new QLabel;
	amountLabel->setMinimumWidth(70);
	amountLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	rowLayout->addWidget(amountLabel);

	auto *bar = new QProgressBar;
	bar->setMinimumWidth(150);
	bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	bar->setTextVisible(false);
	rowLayout->addWidget(bar);

	auto *percentLabel = new QLabel;
	rowLayout->addWidget(percentLabel);

	QPushButton *lock = addButtons(rowLayout, &env, allEnvs);

	layout->addWidget(rowWidget);

	dispatchRows.append({rowWidget, amountLabel, bar, percentLabel, &env, lock});

	updateRow(dispatchRows.last());
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
	amountLabel->setFixedWidth(150);
	amountLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	amountLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	rowLayout->addWidget(amountLabel);
}

void Dispatch::addProgressBar(QHBoxLayout *rowLayout, int amount, int goal)
{
	auto *bar = new QProgressBar;
	bar->setFixedWidth(150);
	bar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	bar->setMaximum(100);

	int percent = (goal > 0) ? (amount * 100 / goal) : 0;
	bar->setValue(std::min(percent, 100));
	bar->setTextVisible(false);
	rowLayout->addWidget(bar);

	auto *percentLabel = new QLabel(QString("%1%").arg(percent));
	percentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	percentLabel->setMinimumWidth(40);
	rowLayout->addWidget(percentLabel);
}

QPushButton *Dispatch::addButtons(QHBoxLayout *rowLayout, Enveloppe *env, EnveloppeManager &allEnvs)
{
	auto *btn = new QPushButton("Remplir 満たす");
	btn->setStyleSheet(fillButtonStyle());
	rowLayout->addWidget(btn);

	auto *plusBtn = new AutoRepeatButton(QIcon(INCREMENT_ICON));
	rowLayout->addWidget(plusBtn);

	auto *minusBtn = new AutoRepeatButton(QIcon(DECREMENT_ICON));
	rowLayout->addWidget(minusBtn);

	auto *lockBtn = new IconButton(QIcon(env->isLocked() ? LOCKED_ICON : UNLOCKED_ICON));
	rowLayout->addWidget(lockBtn);

	connectButtons(btn, plusBtn, minusBtn, lockBtn, env, allEnvs);
	return lockBtn;
}

void Dispatch::connectButtons(QPushButton *fillBtn, QPushButton *plusBtn, QPushButton *minusBtn, QPushButton *lockBtn, Enveloppe *env, EnveloppeManager &allEnvs)
{
	connect(fillBtn, &QPushButton::clicked, this, [this, env, &allEnvs](bool)
	        { fill(env, allEnvs); });

	connect(plusBtn, &QPushButton::clicked, this, [this, env, &allEnvs](bool)
	        { incrementDispatch(env, allEnvs); });

	connect(minusBtn, &QPushButton::clicked, this, [this, env, &allEnvs](bool)
	        { decrementDispatch(env, allEnvs); });

	connect(lockBtn, &QPushButton::clicked, this, [this, env, lockBtn](bool)
	        {
		env->setLocked(!env->isLocked());
		lockBtn->setIcon(QIcon(env->isLocked() ? LOCKED_ICON : UNLOCKED_ICON));
		undoStack.push_back({{0, env, true}});
		redoStack.clear(); });
}

void Dispatch::updateRow(const DispatchRow &row)
{
	int amount = (row.progressBar || row.percentLabel)
	                 ? row.env->getDispatchAmount()
	                 : row.env->getAmount();
	int goal   = row.env->getGoal();

	QLocale jp(QLocale::Japanese, QLocale::Japan);
	QString yen = jp.toString(amount);

	float   rate = getEurJpyRateCached();
	QString eur  = (rate > 0)
	                   ? QLocale(QLocale::French, QLocale::France).toString(amount / rate, 'f', 2) + "€"
	                   : "-€";

	if ( row.amountLabel )
		row.amountLabel->setText(QString("¥%1\n%2").arg(yen, eur));

	if ( row.progressBar )
	{
		int percent = (goal > 0) ? (amount * 100 / goal) : 0;
		row.progressBar->setMaximum(100);
		row.progressBar->setValue(std::min(percent, 100));
	}

	if ( row.percentLabel )
	{
		int percent = (goal > 0) ? (amount * 100 / goal) : 0;
		row.percentLabel->setText(QString("%1%").arg(percent));
	}

	if ( row.lockButton )
		row.lockButton->setIcon(QIcon(row.env->isLocked() ? LOCKED_ICON : UNLOCKED_ICON));
}

void Dispatch::updateAllRows()
{
	updateRow(incomeRow);

	for ( const auto &row : dispatchRows )
		updateRow(row);
}

void Dispatch::fill(Enveloppe *env, EnveloppeManager &allEnvs)
{
	auto &income       = allEnvs.getIncomeEnveloppe();
	int   incomeAmount = income.getAmount();
	int   current      = env->getDispatchAmount();
	int   goal         = env->getGoal();

	if ( current >= goal || incomeAmount <= 0 )
		return;

	int needed      = goal - current;
	int amountToAdd = std::min(needed, incomeAmount);

	env->setDispatchAmount(current + amountToAdd);
	income.setAmount(incomeAmount - amountToAdd);

	undoStack.push_back(
	    {{amountToAdd, env, false},
	     {-amountToAdd, &income, false}});
	redoStack.clear();
	updateAllRows();
}

void Dispatch::incrementDispatch(Enveloppe *env, EnveloppeManager &allEnvs)
{
	auto &income       = allEnvs.getIncomeEnveloppe();
	int   incomeAmount = income.getAmount();
	int   goal         = env->getGoal();
	int   current      = env->getDispatchAmount();

	if ( current >= goal )
		return;

	std::vector<Operation> ops;
	int                    gathered = 0;

	if ( incomeAmount > 0 )
	{
		int onePercent = std::max(1, env->getGoal() / 100);
		int take       = std::min(onePercent, incomeAmount);
		income.setAmount(incomeAmount - take);
		gathered += take;
		ops.push_back({-take, &income, false});
	}

	else
	{
		for ( auto &e : allEnvs.getEnveloppes() )
		{
			if ( &e == env || e.isLocked() )
				continue;

			int onePercent = std::max(1, e.getGoal() / 100);
			int available  = e.getDispatchAmount();
			int take       = std::min(onePercent, available);

			if ( take > 0 )
			{
				e.setDispatchAmount(available - take);
				gathered += take;
				ops.push_back({-take, &e, false});
			}
		}
	}

	if ( gathered > 0 )
	{
		env->setDispatchAmount(env->getDispatchAmount() + gathered);
		ops.push_back({gathered, env, false});
		undoStack.push_back(std::move(ops));
	}

	redoStack.clear();
	updateAllRows();
}

void Dispatch::decrementDispatch(Enveloppe *env, EnveloppeManager &allEnvs)
{
	int current = env->getDispatchAmount();

	if ( current == 0 )
		return;

	int sub      = std::max(1, env->getGoal() / 100);
	int toRemove = std::min(sub, current);

	env->setDispatchAmount(current - toRemove);
	auto &income = allEnvs.getIncomeEnveloppe();
	income.setAmount(income.getAmount() + toRemove);

	undoStack.push_back(
	    {{-toRemove, env, false},
	     {toRemove, &income, false}});
	redoStack.clear();
	updateAllRows();
}

void Dispatch::dispatchIncomeEvenly()
{
	auto &income       = dispatchManagerCopy.getIncomeEnveloppe();
	int   incomeAmount = income.getAmount();

	QList<Enveloppe *> targets;
	int                totalNeeded = 0;

	for ( auto &env : dispatchManagerCopy.getEnveloppes() )
	{
		if ( !env.isLocked() )
		{
			int current = env.getDispatchAmount();
			int goal    = env.getGoal();

			if ( current < goal )
			{
				targets.append(&env);
				totalNeeded += (goal - current);
			}
		}
	}

	if ( totalNeeded == 0 || incomeAmount == 0 )
		return;

	std::vector<Operation> ops;

	if ( incomeAmount >= totalNeeded )
	{
		for ( auto *env : targets )
		{
			int toAdd = env->getGoal() - env->getDispatchAmount();
			env->setDispatchAmount(env->getDispatchAmount() + toAdd);
			ops.push_back({toAdd, env, false});
		}

		income.setAmount(incomeAmount - totalNeeded);
		ops.push_back({-totalNeeded, &income, false});
	}
	else
	{
		int distributed = 0;
		int ratio       = (incomeAmount * 100) / totalNeeded;

		for ( int i = 0; i < targets.size(); ++i )
		{
			auto *env   = targets[i];
			int   need  = env->getGoal() - env->getDispatchAmount();
			int   toAdd = (need * ratio) / 100;

			if ( i == targets.size() - 1 )
				toAdd = incomeAmount - distributed;

			env->setDispatchAmount(env->getDispatchAmount() + toAdd);
			distributed += toAdd;
			ops.push_back({toAdd, env, false});
		}

		income.setAmount(incomeAmount - distributed);
		ops.push_back({-distributed, &income, false});
	}

	redoStack.clear();
	undoStack.push_back(std::move(ops));
	updateAllRows();
}

void Dispatch::apply()
{
	for ( auto &env : dispatchManagerCopy.getEnveloppes() )
	{
		env.setLocked(false);

		env.setAmount(env.getAmount() + env.getDispatchAmount());
		env.setDispatchAmount(0);
	}

	auto &originalEnvs = g_enveloppeManager.getEnveloppes();
	originalEnvs       = dispatchManagerCopy.getEnveloppes();

	auto &originalIncome = g_enveloppeManager.getIncomeEnveloppe();
	auto &copyIncome     = dispatchManagerCopy.getIncomeEnveloppe();
	originalIncome.setAmount(copyIncome.getAmount());

	g_enveloppeManager.saveEnveloppesToJson();
	emit updateNeeded();
}

void Dispatch::undo()
{
	if ( undoStack.empty() )
		return;

	auto lastOps = std::move(undoStack.back());
	undoStack.pop_back();

	redoStack.push_back(lastOps);

	for ( const auto &op : lastOps )
	{
		if ( op.lockSwitch )
			op.env->setLocked(!op.env->isLocked());
		else if ( op.env->getName() == "Revenus\n収入" )
			op.env->setAmount(op.env->getAmount() - op.amountAdded);
		else
			op.env->setDispatchAmount(op.env->getDispatchAmount() - op.amountAdded);
	}

	updateAllRows();
}

void Dispatch::redo()
{
	if ( redoStack.empty() )
		return;

	auto ops = std::move(redoStack.back());
	redoStack.pop_back();

	undoStack.push_back(ops);

	for ( const auto &op : ops )
	{
		if ( op.lockSwitch )
			op.env->setLocked(!op.env->isLocked());

		else if ( op.env->getName() == "Revenus\n収入" )
			op.env->setAmount(op.env->getAmount() + op.amountAdded);
		else
			op.env->setDispatchAmount(op.env->getDispatchAmount() + op.amountAdded);
	}

	updateAllRows();
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
