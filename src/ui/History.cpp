#include "History.hpp"
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
#include <QVariant>
#include <QMetaType>
#include <QFrame>
#include <QAbstractItemView>

#include <algorithm>
#include <optional>
#include <ranges>
#include <string>
#include <format>

History::History(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignTop);
	setLayout(mainLayout);

	selected = "Tous すべて";

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);
	mainLayout->addWidget(scrollArea);

	showHistory();
}

void History::showHistory()
{
	updateGlobalDateRange();
	clearTopBarContainer();

	topBarContainer = new QWidget(this);
	topBar = new QHBoxLayout(topBarContainer);
	topBar->setAlignment(Qt::AlignLeft);
	topBar->addLayout(createEnveloppeDropdown());
	topBar->addLayout(createDateDropdowns());
	mainLayout->insertWidget(0, topBarContainer);

	tableContainer = new QWidget;
	QVBoxLayout* tableLayout = new QVBoxLayout(tableContainer);

	QTableWidget* table = new QTableWidget(tableContainer);
	setUpTable(table);
	populateTable(table);
	table->resizeColumnsToContents();
	tableLayout->addWidget(table);

	scrollArea->setWidget(tableContainer);
}

void History::clearTopBarContainer()
{
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

void History::setUpTable(QTableWidget* table)
{
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({ "Date 日付", "Montant 金額", "Enveloppe 封筒", "Description 説明" });
	table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setStretchLastSection(true);
	table->setTextElideMode(Qt::ElideNone);
	table->setWordWrap(false);
	table->setRowCount(0);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setVisible(false);
	table->horizontalHeader()->setHighlightSections(false);
	table->setShowGrid(true);
	table->setGridStyle(Qt::SolidLine);
	table->setStyleSheet(setTableStyleSheet());
}

QString History::setTableStyleSheet()
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

void History::populateTable(QTableWidget* table)
{
	if(!table)
		return;

	std::vector<Expense> displayList = collectFilteredExpenses();
	std::ranges::sort(displayList, [](const Expense & a, const Expense & b)
	{
		return a.date > b.date;
	});

	fillTableWithExpenses(table, displayList);
}

std::vector<Expense> History::collectFilteredExpenses()
{
	std::vector<Expense> result;

	for(const auto& env : g_enveloppeManager.getEnveloppes())
	{
		if(selected != "Tous すべて" && env.getName() != selected)
			continue;

		for(const auto& exp : env.getExpenses())
		{
			auto ym = std::chrono::year_month{ exp.date.year(), exp.date.month() };

			if(ym < startDate || ym > endDate)
				continue;

			result.push_back(exp);
		}
	}

	return result;
}

void History::fillTableWithExpenses(QTableWidget* table, const std::vector<Expense>& expenses)
{
	QLocale jp(QLocale::Japanese, QLocale::Japan);

	for(const auto& exp : expenses)
	{
		int row = table->rowCount();
		table->insertRow(row);

		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(std::format("{}", exp.date))));

		auto* amountItem = new QTableWidgetItem(QString("¥%1").arg(jp.toString(exp.amount)));
		amountItem->setForeground(QBrush(QColor(exp.amount < 0 ? "#FA5E57" : "#337BFF")));
		table->setItem(row, 1, amountItem);

		QString name = QString::fromStdString(exp.enveloppe).replace('\n', ' ');
		table->setItem(row, 2, new QTableWidgetItem(name));
		table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(exp.info)));
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

	QLabel* enveloppeLabel = new QLabel("Enveloppe 封筒:", this);
	enveloppeLabel->setStyleSheet("color: #E1E1E2; font-family: 'Helvetica Neue';");

	enveloppeCombo = new QComboBox(this);
	enveloppeCombo->setStyleSheet(setUpComboStyleSheet());

	enveloppeCombo->addItem("Tous すべて");

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

QString History::setUpComboStyleSheet()
{
	return QString(R"(
		QComboBox {
			background-color: #1B272A;
			color: #E1E1E2;
			border: 1px solid #444;
			border-radius: 4px;
			padding-left: 6px;
			min-height: 28px;
			min-width: 100;
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

QHBoxLayout* History::createDateDropdowns()
{
	static bool registered = []
	{
		qRegisterMetaType<std::chrono::year_month>("std::chrono::year_month");
		return true;
	}();
	(void)registered;

	setupDateDropdownWidgets();
	initDateCombos();
	connectDateComboSignals();

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

void History::setupDateDropdownWidgets()
{
	startCombo = new QComboBox(this);
	startCombo->setStyleSheet(setUpComboStyleSheet());

	endCombo = new QComboBox(this);
	endCombo->setStyleSheet(startCombo->styleSheet());
}

void History::initDateCombos()
{
	bool initStart = (startDate == std::chrono::year_month{});
	bool initEnd = (endDate == std::chrono::year_month{});

	for(auto ym = globalStartDate; ym <= globalEndDate; ym += std::chrono::months{1})
	{
		QString label = QString::number(unsigned(ym.month())) + "/" + QString::number(int(ym.year()));
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
}

void History::connectDateComboSignals()
{
	connect(startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	{
		startDate = startCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory);
	});
	connect(endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	{
		endDate = endCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory);
	});
}
