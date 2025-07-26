#pragma once

#include <QWidget>
#include <QString>
#include <string>
#include <chrono>

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QScrollArea;
class QTableWidget;
class QLabel;

class History : public QWidget
{
	Q_OBJECT

public:

	explicit History(QWidget* parent = nullptr);
	void showHistory();

private:

	std::chrono::year_month globalStartDate;
	std::chrono::year_month globalEndDate;
	std::chrono::year_month startDate;
	std::chrono::year_month endDate;
	std::string selected;

	QComboBox* enveloppeCombo;
	QComboBox* startCombo;
	QComboBox* endCombo;
	QVBoxLayout* mainLayout;
	QHBoxLayout* topBar;
	QWidget* topBarContainer = nullptr;
	QScrollArea* scrollArea = nullptr;

	QHBoxLayout* createDateDropdowns();
	QHBoxLayout* createEnveloppeDropdown();
	void updateGlobalDateRange();
	void clearHistoryPage();
	void populateTable(QTableWidget* table);
};
