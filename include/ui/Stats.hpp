#pragma once

#include <QWidget>
#include <QString>
#include <chrono>
#include <tuple>

class Enveloppe;

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QScrollArea;
class QTableWidget;

class Stats : public QWidget
{
	Q_OBJECT

public:
	explicit Stats(QWidget* parent = nullptr);
	void showStats();

private:
	QVBoxLayout* mainLayout;
	QHBoxLayout* topBar;
	QWidget* topBarContainer = nullptr;
	QScrollArea* scrollArea = nullptr;
	QWidget* tableContainer = nullptr;
	QComboBox* startCombo;
	QComboBox* endCombo;

	std::chrono::year_month globalStartDate;
	std::chrono::year_month globalEndDate;
	std::chrono::year_month startDate;
	std::chrono::year_month endDate;

	void updateGlobalDateRange();
	void clearStatsPage();

	void setUpTable(QTableWidget* table);
	void populateTable(QTableWidget* table);
	std::tuple<int, int, int> calculateExpenseStats(const Enveloppe& env);
	QString setTableStyleSheet();

	QHBoxLayout* createDateDropdowns();
	void setupDateWidgets();
	void initAndFillCombos();
	void connectDateSignals();
	QString setUpComboStyleSheet();
};
