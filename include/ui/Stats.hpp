#pragma once

#include <QWidget>
#include <QString>
#include <chrono>

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

	std::chrono::year_month globalStartDate;
	std::chrono::year_month globalEndDate;
	std::chrono::year_month startDate;
	std::chrono::year_month endDate;

	QComboBox* startCombo;
	QComboBox* endCombo;
	QVBoxLayout* mainLayout;
	QHBoxLayout* topBar;
	QWidget* topBarContainer = nullptr;
	QScrollArea* scrollArea = nullptr;

	QHBoxLayout* createDateDropdowns();
	void updateGlobalDateRange();
	void clearStatsPage();
	void populateTable(QTableWidget* table);
};
