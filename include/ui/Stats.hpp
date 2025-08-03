#pragma once

#include <QString>
#include <QWidget>
#include <chrono>
#include <tuple>

class Enveloppe;
class KakeiboScrollArea;

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QTableWidget;

class Stats : public QWidget
{
	Q_OBJECT

  public:
	explicit Stats(QWidget *parent = nullptr);
	void showStats();

  private:
	QVBoxLayout *mainLayout;
	QHBoxLayout *topBar;
	QWidget     *topBarContainer = nullptr;
	QWidget     *tableContainer  = nullptr;
	QComboBox   *startCombo;
	QComboBox   *endCombo;

	std::chrono::year_month globalStartDate;
	std::chrono::year_month globalEndDate;
	std::chrono::year_month startDate;
	std::chrono::year_month endDate;

	void updateGlobalDateRange();
	void clearStatsPage();

	void                      populateTable(QTableWidget *table);
	std::tuple<int, int, int> calculateExpenseStats(const Enveloppe &env);

	QHBoxLayout *createDateDropdowns();
	void         setupDateWidgets();
	void         initAndFillCombos();
	void         connectDateSignals();
	QString      setUpComboStyleSheet();
};
