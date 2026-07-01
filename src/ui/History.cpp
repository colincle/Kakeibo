#include "History.hpp"
#include "Assets.hpp"
#include "DateRange.hpp"
#include "EnvelopeManager.hpp"
#include "Globals.hpp"
#include "KakeiboScrollArea.hpp"
#include "KakeiboTable.hpp"
#include "Theme.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMetaType>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>

#include <algorithm>
#include <format>
#include <optional>
#include <ranges>
#include <string>

History::History(QWidget *parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignTop);
	setLayout(mainLayout);

	selected = "Tous すべて";
	showHistory();
}

void History::showHistory()
{
	updateGlobalDateRange();
	clearTopBarContainer();

	topBarContainer = new QWidget(this);
	topBar          = new QHBoxLayout(topBarContainer);
	topBar->setAlignment(Qt::AlignLeft);
	topBar->addLayout(createEnvelopeDropdown());
	topBar->addLayout(createDateDropdowns());
	mainLayout->insertWidget(0, topBarContainer);

	tableContainer           = new QWidget;
	QVBoxLayout *tableLayout = new QVBoxLayout(tableContainer);

	KakeiboTable *table = new KakeiboTable(tableContainer);
	setUpTable(table);
	populateTable(table);
	table->resizeColumnsToContents();
	tableLayout->addWidget(table);
	mainLayout->addWidget(tableContainer);
}

void History::clearTopBarContainer()
{
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
}

void History::clearHistoryPage()
{
	if ( topBarContainer )
	{
		mainLayout->removeWidget(topBarContainer);
		delete topBarContainer;
		topBarContainer = nullptr;
	}
}

void History::setUpTable(KakeiboTable *table)
{
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({"Date 日付", "Montant 金額", "Enveloppe 封筒", "Description 説明"});
	table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(table, &KakeiboTable::customContextMenuRequested, this, [this, table](const QPoint &pos)
	        {
		int row = table->rowAt(pos.y());

		if(row < 0)
			return;

		QString date = table->item(row, 0)->data(Qt::UserRole).toString();
		QString amount = table->item(row, 1)->data(Qt::UserRole).toString();
		QString envelope = table->item(row, 2)->data(Qt::UserRole).toString();
		QString desc = table->item(row, 3)->data(Qt::UserRole).toString();

		handleRightClick(date, amount, envelope, desc); });
}

void History::handleRightClick(const QString &date, const QString &amount, const QString &envelope, const QString &desc)
{
	QMenu *menu = new QMenu(this);
	menu->setStyleSheet(R"(
		QMenu::separator {
			height: 1px;
			background: black;
			margin: 4px 0;
		}
	)");

	QMenu *moveMenu = menu->addMenu("Déplacer vers une autre enveloppe / 別の封筒に移動");

	for ( const auto &env : g_envelopeManager.getEnvelopes() )
	{
		QString name = QString::fromStdString(env.getName());

		if ( name == envelope )
			continue;

		QAction *action = moveMenu->addAction(name);
		moveMenu->addSeparator();
		connect(action, &QAction::triggered, this, [=, this]()
		        {
			g_envelopeManager.moveExpenseToNewEnvelope(date, amount, envelope, desc, name);
			emit updateNeeded(); });
	}

	menu->addSeparator();
	QAction *forgetAction = menu->addAction("Oublier le type de dépense / 支出タイプを忘れる");
	menu->addSeparator();
	QAction *deleteAction = menu->addAction("Supprimer / 削除");

	QAction *selection = menu->exec(QCursor::pos());

	if ( selection == forgetAction )
	{
		g_envelopeManager.forgetExpenseType(envelope, desc);
		emit updateNeeded();
	}
	else if ( selection == deleteAction )
	{
		g_envelopeManager.deleteExpense(date, amount, envelope, desc);
		emit updateNeeded();
	}
}

void History::populateTable(KakeiboTable *table)
{
	if ( !table )
		return;

	std::vector<Expense> displayList = collectFilteredExpenses();
	std::ranges::sort(displayList, [](const Expense &a, const Expense &b)
	                  { return a.date > b.date; });

	fillTableWithExpenses(table, displayList);
}

std::vector<Expense> History::collectFilteredExpenses()
{
	std::vector<Expense> result;

	for ( const auto &env : g_envelopeManager.getEnvelopes() )
	{
		if ( selected != "Tous すべて" && env.getName() != selected )
			continue;

		for ( const auto &exp : env.getExpenses() )
		{
			auto ym = std::chrono::year_month {exp.date.year(), exp.date.month()};

			if ( ym < startDate || ym > endDate )
				continue;

			result.push_back(exp);
		}
	}

	return result;
}

void History::fillTableWithExpenses(KakeiboTable *table, const std::vector<Expense> &expenses)
{
	QLocale jp(QLocale::Japanese, QLocale::Japan);

	for ( const auto &exp : expenses )
	{
		int row = table->rowCount();
		table->insertRow(row);

		QString originalDate = QString::fromStdString(std::format("{}", exp.date));
		auto   *dateItem     = new QTableWidgetItem(originalDate);
		dateItem->setData(Qt::UserRole, originalDate);
		table->setItem(row, 0, dateItem);

		QString formattedAmount = QString("¥%1").arg(jp.toString(exp.amount));
		auto   *amountItem      = new QTableWidgetItem(formattedAmount);
		amountItem->setForeground(QBrush(QColor(exp.amount < 0 ? "#FA5E57" : "#337BFF")));
		amountItem->setData(Qt::UserRole, QString::number(exp.amount));
		table->setItem(row, 1, amountItem);

		QString originalEnv = QString::fromStdString(exp.envelope);
		QString displayEnv  = originalEnv;
		displayEnv.replace('\n', ' ');
		auto *envItem = new QTableWidgetItem(displayEnv);
		envItem->setData(Qt::UserRole, originalEnv);
		table->setItem(row, 2, envItem);

		QString originalInfo = QString::fromStdString(exp.info);
		auto   *descItem     = new QTableWidgetItem(originalInfo);
		descItem->setData(Qt::UserRole, originalInfo);
		table->setItem(row, 3, descItem);
	}
}

void History::updateGlobalDateRange()
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

QHBoxLayout *History::createEnvelopeDropdown()
{
	QHBoxLayout *layout = new QHBoxLayout();

	QLabel *envelopeLabel = new QLabel("Enveloppe 封筒:", this);
	envelopeLabel->setStyleSheet("color: #E1E1E2; font-family: 'Helvetica Neue';");

	envelopeCombo = new QComboBox(this);
	envelopeCombo->setStyleSheet(setUpComboStyleSheet());

	envelopeCombo->addItem("Tous すべて");

	for ( const auto &env : g_envelopeManager.getEnvelopes() )
		envelopeCombo->addItem(QString::fromStdString(env.getName()));

	if ( !selected.empty() )
	{
		int index = envelopeCombo->findText(QString::fromStdString(selected));

		if ( index != -1 )
			envelopeCombo->setCurrentIndex(index);
	}

	connect(envelopeCombo, &QComboBox::currentTextChanged, [this](const QString &text)
	        {
		selected = text.toStdString();
		QTimer::singleShot(0, this, &History::showHistory); });

	layout->addWidget(envelopeLabel);
	layout->addWidget(envelopeCombo);

	return layout;
}

QString History::setUpComboStyleSheet()
{
	return Theme::comboBoxStyle(DOWN_ICON);
}

QHBoxLayout *History::createDateDropdowns()
{
	static bool registered = []
	{
		qRegisterMetaType<std::chrono::year_month>("std::chrono::year_month");
		return true;
	}();
	(void) registered;

	setupDateDropdownWidgets();
	initDateCombos();
	connectDateComboSignals();

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

void History::setupDateDropdownWidgets()
{
	startCombo = new QComboBox(this);
	startCombo->setStyleSheet(setUpComboStyleSheet());

	endCombo = new QComboBox(this);
	endCombo->setStyleSheet(startCombo->styleSheet());
}

void History::initDateCombos()
{
	bool initStart = (startDate == std::chrono::year_month {});
	bool initEnd   = (endDate == std::chrono::year_month {});

	for ( auto ym = globalStartDate; ym <= globalEndDate; ym += std::chrono::months {1} )
	{
		QString  label = QString::number(unsigned(ym.month())) + "/" + QString::number(int(ym.year()));
		QVariant data  = QVariant::fromValue(ym);
		startCombo->addItem(label, data);
		endCombo->addItem(label, data);
	}

	if ( initStart )
		startDate = globalStartDate;

	if ( initEnd )
		endDate = globalEndDate;

	int startIdx = startCombo->findData(QVariant::fromValue(startDate));

	if ( startIdx != -1 )
		startCombo->setCurrentIndex(startIdx);

	int endIdx = endCombo->findData(QVariant::fromValue(endDate));

	if ( endIdx != -1 )
		endCombo->setCurrentIndex(endIdx);
}

void History::connectDateComboSignals()
{
	connect(startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	        {
		startDate = startCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory); });
	connect(endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
	        {
		endDate = endCombo->itemData(index).value<std::chrono::year_month>();
		QTimer::singleShot(0, this, &History::showHistory); });
}
