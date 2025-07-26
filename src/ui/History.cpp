#include "History.hpp"
#include "Globals.hpp"

#include <algorithm>
#include <optional>
#include <ranges>
#include <string>
#include <format>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QVariant>
#include <QMetaType>

History::History(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignTop);
	setLayout(mainLayout);

	selected = "Toutes les enveloppes";

	showHistory();
}

void History::showHistory()
{
	updateGlobalDateRange();
	clearHistoryPage();
	topBarContainer = new QWidget(this);
	topBar = new QHBoxLayout(topBarContainer);
	topBar->setAlignment(Qt::AlignLeft);
	topBar->addLayout(createEnveloppeDropdown());
	topBar->addLayout(createDateDropdowns());
	mainLayout->addWidget(topBarContainer);

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	QWidget* tableContainer = new QWidget(scrollArea);
	QVBoxLayout* tableLayout = new QVBoxLayout(tableContainer);

	QTableWidget* table = new QTableWidget(tableContainer);
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({ "Date", "Montant", "Enveloppe", "Description" });
	table->horizontalHeader()->setStretchLastSection(true);
	table->setStyleSheet("QTableWidget { color: black; background-color: white; }"
	                     "QHeaderView::section { background-color: lightgray; color: black; }");

	populateTable(table);
	tableLayout->addWidget(table);

	scrollArea->setWidget(tableContainer);
	mainLayout->addWidget(scrollArea);
}

void History::populateTable(QTableWidget* table)
{
	if(!table)
		return;

	table->setRowCount(0);
	table->setStyleSheet("QTableWidget { color: black; background-color: white; selection-background-color: lightblue; }"
	                     "QHeaderView::section { background-color: lightgray; color: black; }");

	std::vector<Expense> displayList;

	for(const auto& env : g_enveloppeManager.getEnveloppes())
	{
		if(selected != "Toutes les enveloppes" && env.getName() != selected)
			continue;

		for(const auto& exp : env.getExpenses())
		{
			auto ym = std::chrono::year_month{ exp.date.year(), exp.date.month() };

			if(ym < startDate || ym > endDate)
				continue;

			displayList.push_back(exp);
		}
	}

	std::ranges::sort(displayList, [](const Expense & a, const Expense & b)
	{
		return a.date > b.date;
	});

	for(const auto& exp : displayList)
	{
		int row = table->rowCount();
		table->insertRow(row);
		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(std::format("{}", exp.date))));
		table->setItem(row, 1, new QTableWidgetItem(QString::number(exp.amount)));
		table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(exp.enveloppe)));
		table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(exp.info)));
	}
}

void History::clearHistoryPage()
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

void History::updateGlobalDateRange()
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

QHBoxLayout* History::createEnveloppeDropdown()
{
	QHBoxLayout* layout = new QHBoxLayout();

	QLabel* enveloppeLabel = new QLabel("Enveloppe:", this);
	enveloppeLabel->setStyleSheet("color: black;");
	enveloppeCombo = new QComboBox(this);
	enveloppeCombo->setStyleSheet("color: black;");

	enveloppeCombo->addItem("Toutes les enveloppes");

	for(const auto& env : g_enveloppeManager.getEnveloppes())
		enveloppeCombo->addItem(QString::fromStdString(env.getName()));

	if(!selected.empty())
	{
		int index = enveloppeCombo->findText(QString::fromStdString(selected));

		if(index != -1)
			enveloppeCombo->setCurrentIndex(index);
	}

	connect(enveloppeCombo, &QComboBox::currentTextChanged, [this](const QString & text)
	{
		selected = text.toStdString();
		QTimer::singleShot(0, this, &History::showHistory);
	});

	layout->addWidget(enveloppeLabel);
	layout->addWidget(enveloppeCombo);

	return layout;
}

QHBoxLayout* History::createDateDropdowns() //refactor
{
	static bool registered = []
	{
		qRegisterMetaType<std::chrono::year_month>("std::chrono::year_month");
		return true;
	}();
	(void)registered;

	QHBoxLayout* layout = new QHBoxLayout();

	QLabel* startLabel = new QLabel("Début:", this);
	startLabel->setStyleSheet("color: black;");
	startCombo = new QComboBox(this);
	startCombo->setStyleSheet("color: black;");

	QLabel* endLabel = new QLabel("Fin:", this);
	endLabel->setStyleSheet("color: black;");
	endCombo = new QComboBox(this);
	endCombo->setStyleSheet("color: black;");

	bool initStart = (startDate == std::chrono::year_month{});
	bool initEnd = (endDate == std::chrono::year_month{});

	for(auto ym = globalStartDate; ym <= globalEndDate; ym += std::chrono::months{1})
	{
		QString label = QString::number(static_cast<unsigned>(ym.month())) + "/" + QString::number(int(ym.year()));
		QVariant data = QVariant::fromValue(ym);
		startCombo->addItem(label, data);
		endCombo->addItem(label, data);
	}

	if(initStart)
		startDate = globalStartDate;

	if(initEnd)
		endDate = globalEndDate;

	int startIdx = startCombo->findData(QVariant::fromValue(startDate));

	if(startIdx != -1)
		startCombo->setCurrentIndex(startIdx);

	int endIdx = endCombo->findData(QVariant::fromValue(endDate));

	if(endIdx != -1)
		endCombo->setCurrentIndex(endIdx);

	connect(startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ =, this](int index)
	{
		startDate = startCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory);
	});

	connect(endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ =, this](int index)
	{
		endDate = endCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory);
	});

	layout->addSpacing(20);
	layout->addWidget(startLabel);
	layout->addWidget(startCombo);
	layout->addSpacing(20);
	layout->addWidget(endLabel);
	layout->addWidget(endCombo);

	return layout;
}
