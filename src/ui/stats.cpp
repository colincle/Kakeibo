#include "Stats.hpp"
#include "Globals.hpp"
#include "Assets.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QLocale>
#include <QVariant>
#include <QMetaType>
#include <QFrame>
#include <QAbstractItemView>

#include <algorithm>
#include <optional>
#include <ranges>
#include <format>

Stats::Stats(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignTop);
	setLayout(mainLayout);

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);
	mainLayout->addWidget(scrollArea);

	showStats();
}

void Stats::showStats()
{
	updateGlobalDateRange();

	if(topBarContainer)
	{
		mainLayout->removeWidget(topBarContainer);
		delete topBarContainer;
		topBarContainer = nullptr;
	}

	if(tableContainer)
	{
		scrollArea->takeWidget();
		delete tableContainer;
		tableContainer = nullptr;
	}

	topBarContainer = new QWidget(this);
	topBar = new QHBoxLayout(topBarContainer);
	topBar->setAlignment(Qt::AlignLeft);
	topBar->addLayout(createDateDropdowns());
	mainLayout->insertWidget(0, topBarContainer);

	tableContainer = new QWidget;
	QVBoxLayout* tableLayout = new QVBoxLayout(tableContainer);

	QTableWidget* table = new QTableWidget(tableContainer);
	setUpTable(table);
	tableLayout->addWidget(table);

	scrollArea->setWidget(tableContainer);
}

void Stats::setUpTable(QTableWidget* table)
{
	table->setColumnCount(3);
	table->setHorizontalHeaderLabels({ "Enveloppe 封筒", "Par mois 月額", "Par année 年額" });
	table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setStretchLastSection(true);
	table->setTextElideMode(Qt::ElideNone);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setWordWrap(false);
	table->verticalHeader()->setVisible(false);
	populateTable(table);
	table->resizeColumnsToContents();
	table->setStyleSheet(setTableStyleSheet());
}

void Stats::populateTable(QTableWidget* table)
{
	if(!table)
		return;

	for(const auto& env : g_enveloppeManager.getEnveloppes())
	{
		auto [total, perMonthAvg, perYearAvg] = calculateExpenseStats(env);

		if(total == 0)
			continue;

		int row = table->rowCount();
		table->insertRow(row);
		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(env.getName()).replace('\n', ' ')));
		QLocale jp(QLocale::Japanese, QLocale::Japan);
		auto* perMonthItem = new QTableWidgetItem("¥" + jp.toString(perMonthAvg));
		table->setItem(row, 1, perMonthItem);

		if(perYearAvg != -1)
		{
			auto* perYearItem = new QTableWidgetItem("¥" + jp.toString(perYearAvg));
			table->setItem(row, 2, perYearItem);
		}
		else
			table->setItem(row, 2, new QTableWidgetItem("n/a"));
	}
}

QString Stats::setTableStyleSheet()
{
	return R"(
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
	       )";
}

QHBoxLayout* Stats::createDateDropdowns()
{
	static bool registered = []
	{
		qRegisterMetaType<std::chrono::year_month>("std::chrono::year_month");
		return true;
	}();
	(void)registered;

	setupDateWidgets();
	initAndFillCombos();
	connectDateSignals();

	QHBoxLayout* layout = new QHBoxLayout();
	QLabel* startLabel = new QLabel("Début 開始:", this);
	startLabel->setStyleSheet("color: #E1E1E2;");
	QLabel* endLabel = new QLabel("Fin 終了:", this);
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
	if(startDate == std::chrono::year_month{})
		startDate = globalStartDate;

	if(endDate == std::chrono::year_month{})
		endDate = globalEndDate;

	for(auto ym = globalStartDate; ym <= globalEndDate; ym += std::chrono::months{1})
	{
		QString label = QString::number(unsigned(ym.month())) + "/" + QString::number(int(ym.year()));
		QVariant data = QVariant::fromValue(ym);
		startCombo->addItem(label, data);
		endCombo->addItem(label, data);
	}

	int startIdx = startCombo->findData(QVariant::fromValue(startDate));

	if(startIdx != -1)
		startCombo->setCurrentIndex(startIdx);

	int endIdx = endCombo->findData(QVariant::fromValue(endDate));

	if(endIdx != -1)
		endCombo->setCurrentIndex(endIdx);
}

void Stats::connectDateSignals()
{
	connect(startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	{
		startDate = startCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &Stats::showStats);
	});
	connect(endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	{
		endDate = endCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &Stats::showStats);
	});
}

QString Stats::setUpComboStyleSheet()
{
	return QString(R"(
			QComboBox {
				background-color: #1B272A;
				color: #E1E1E2;
				border: 1px solid #444;
				border-radius: 4px;
				padding-left: 6px;
				min-height: 28px;
				min-width: 70px;
			}
			QComboBox::drop-down {
				subcontrol-origin: padding;
				subcontrol-position: top right;
				width: 25px;
				border-left: 1px solid #444;
				background-color: #2F3D41;
			}
			QComboBox::down-arrow {
				image: url(%1);
				width: 12px;
				height: 12px;
			}
		)").arg(DOWN_ICON);
}

void Stats::updateGlobalDateRange()
{
	static bool firstTime = true;
	std::optional<std::chrono::sys_days> minDate, maxDate;

	for(const auto& env : g_enveloppeManager.getEnveloppes())
	{
		for(const auto& exp : env.getExpenses())
		{
			if(!minDate || exp.date < *minDate)
				minDate = exp.date;

			if(!maxDate || exp.date > *maxDate)
				maxDate = exp.date;
		}
	}

	if(minDate && maxDate)
	{
		auto ymdMin = std::chrono::year_month_day{*minDate};
		globalStartDate = std::chrono::year_month{ymdMin.year(), ymdMin.month()};

		auto ymdMax = std::chrono::year_month_day{*maxDate};
		globalEndDate = std::chrono::year_month{ymdMax.year(), ymdMax.month()};

		if(firstTime)
		{
			startDate = globalStartDate;
			endDate = globalEndDate;
			firstTime = false;
		}
	}
}

std::tuple<int, int, int> Stats::calculateExpenseStats(const Enveloppe& env)
{
	int total = 0;
	std::map<std::pair<int, unsigned>, int> perMonth;

	for(const auto& exp : env.getExpenses())
	{
		auto ym = std::chrono::year_month{ exp.date.year(), exp.date.month() };

		if(ym < startDate || ym > endDate)
			continue;

		total += exp.amount;
		perMonth[ { int(exp.date.year()), unsigned(exp.date.month()) }] += exp.amount;
	}

	int monthCount = static_cast<int>(perMonth.size());
	int perMonthAvg = monthCount ? total / monthCount : 0;
	int perYearAvg = monthCount >= 12 ? (total * 12) / monthCount : -1;

	return { total, perMonthAvg, perYearAvg };
}

void Stats::clearStatsPage()
{
	if(topBarContainer)
	{
		mainLayout->removeWidget(topBarContainer);
		delete topBarContainer;
		topBarContainer = nullptr;
	}

	if(scrollArea)
	{
		mainLayout->removeWidget(scrollArea);
		delete scrollArea;
		scrollArea = nullptr;
	}
}
