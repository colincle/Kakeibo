#include "Stats.hpp"
#include "Assets.hpp"
#include "DateRange.hpp"
#include "Globals.hpp"
#include "KakeiboScrollArea.hpp"
#include "KakeiboTable.hpp"
#include "Theme.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QMetaType>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>

#include <algorithm>
#include <format>
#include <optional>
#include <ranges>

Stats::Stats(QWidget *parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignTop);
	setLayout(mainLayout);
	showStats();
}

void Stats::showStats()
{
	updateGlobalDateRange();

	if ( topBarContainer )
	{
		mainLayout->removeWidget(topBarContainer);
		delete topBarContainer;
		topBarContainer = nullptr;
	}

	if ( tableContainer )
	{
		delete tableContainer;
		tableContainer = nullptr;
	}

	topBarContainer = new QWidget(this);
	topBar          = new QHBoxLayout(topBarContainer);
	topBar->setAlignment(Qt::AlignLeft);
	topBar->addLayout(createDateDropdowns());
	mainLayout->insertWidget(0, topBarContainer);

	tableContainer           = new QWidget;
	QVBoxLayout *tableLayout = new QVBoxLayout(tableContainer);

	auto *table = new KakeiboTable(tableContainer);
	table->setColumnCount(3);
	table->setHorizontalHeaderLabels({"Enveloppe 封筒", "Par mois 月額", "Par année 年額"});
	populateTable(table);
	table->resizeColumnsToContents();
	tableLayout->addWidget(table);
	mainLayout->addWidget(tableContainer);
}

void Stats::populateTable(QTableWidget *table)
{
	if ( !table )
		return;

	for ( const auto &env : g_envelopeManager.getEnvelopes() )
	{
		auto [total, perMonthAvg, perYearAvg] = calculateExpenseStats(env);

		if ( total == 0 )
			continue;

		int row = table->rowCount();
		table->insertRow(row);
		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(env.getName()).replace('\n', ' ')));
		QLocale jp(QLocale::Japanese, QLocale::Japan);
		auto   *perMonthItem = new QTableWidgetItem("¥" + jp.toString(perMonthAvg));
		table->setItem(row, 1, perMonthItem);

		if ( perYearAvg != -1 )
		{
			auto *perYearItem = new QTableWidgetItem("¥" + jp.toString(perYearAvg));
			table->setItem(row, 2, perYearItem);
		}
		else
			table->setItem(row, 2, new QTableWidgetItem("n/a"));
	}
}

QHBoxLayout *Stats::createDateDropdowns()
{
	static bool registered = []
	{
		qRegisterMetaType<std::chrono::year_month>("std::chrono::year_month");
		return true;
	}();
	(void) registered;

	setupDateWidgets();
	initAndFillCombos();
	connectDateSignals();

	QHBoxLayout *layout     = new QHBoxLayout();
	QLabel      *startLabel = new QLabel("Début 開始:", this);
	startLabel->setStyleSheet("color: #E1E1E2;");
	QLabel *endLabel = new QLabel("Fin 終了:", this);
	endLabel->setStyleSheet("color: #E1E1E2;");

	layout->addSpacing(20);
	layout->addWidget(startLabel);
	layout->addWidget(startCombo);
	layout->addSpacing(20);
	layout->addWidget(endLabel);
	layout->addWidget(endCombo);

	return layout;
}

void Stats::setupDateWidgets()
{
	startCombo = new QComboBox(this);
	startCombo->setStyleSheet(setUpComboStyleSheet());

	endCombo = new QComboBox(this);
	endCombo->setStyleSheet(startCombo->styleSheet());
}

void Stats::initAndFillCombos()
{
	if ( startDate == std::chrono::year_month {} )
		startDate = globalStartDate;

	if ( endDate == std::chrono::year_month {} )
		endDate = globalEndDate;

	for ( auto ym = globalStartDate; ym <= globalEndDate; ym += std::chrono::months {1} )
	{
		QString  label = QString::number(unsigned(ym.month())) + "/" + QString::number(int(ym.year()));
		QVariant data  = QVariant::fromValue(ym);
		startCombo->addItem(label, data);
		endCombo->addItem(label, data);
	}

	int startIdx = startCombo->findData(QVariant::fromValue(startDate));

	if ( startIdx != -1 )
		startCombo->setCurrentIndex(startIdx);

	int endIdx = endCombo->findData(QVariant::fromValue(endDate));

	if ( endIdx != -1 )
		endCombo->setCurrentIndex(endIdx);
}

void Stats::connectDateSignals()
{
	connect(startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	        {
		startDate = startCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &Stats::showStats); });
	connect(endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	        {
		endDate = endCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &Stats::showStats); });
}

QString Stats::setUpComboStyleSheet()
{
	return Theme::comboBoxStyle(DOWN_ICON);
}

void Stats::updateGlobalDateRange()
{
	static bool firstTime = true;

	if ( auto range = expenseDateRange() )
	{
		globalStartDate = range->first;
		globalEndDate   = range->second;

		if ( firstTime )
		{
			startDate = globalStartDate;
			endDate   = globalEndDate;
			firstTime = false;
		}
	}
}

std::tuple<int, int, int> Stats::calculateExpenseStats(const Envelope &env)
{
	int total = 0;

	for ( const auto &exp : env.getExpenses() )
	{
		auto ym = std::chrono::year_month {exp.date.year(), exp.date.month()};

		if ( ym < startDate || ym > endDate )
			continue;

		total += exp.amount;
	}

	int monthCount  = static_cast<int>((endDate - startDate) / std::chrono::months {1}) + 1;
	int perMonthAvg = monthCount > 0 ? total / monthCount : 0;
	int perYearAvg  = monthCount > 0 ? (total * 12) / monthCount : -1;

	return {total, perMonthAvg, perYearAvg};
}

void Stats::clearStatsPage()
{
	if ( !topBarContainer )
		return;

	mainLayout->removeWidget(topBarContainer);
	delete topBarContainer;
	topBarContainer = nullptr;
}
