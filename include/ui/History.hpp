#pragma once

#include <QString>
#include <QWidget>
#include <chrono>
#include <string>
#include <vector>

class KakeiboScrollArea;

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class KakeiboTable;
class QLabel;

struct Expense;

class History : public QWidget
{
	Q_OBJECT

  signals:
	void updateNeeded();

  public:
	explicit History(QWidget *parent = nullptr);
	void showHistory();

  private:
	std::chrono::year_month globalStartDate;
	std::chrono::year_month globalEndDate;
	std::chrono::year_month startDate;
	std::chrono::year_month endDate;

	std::string selected;

	QComboBox   *envelopeCombo;
	QComboBox   *startCombo;
	QComboBox   *endCombo;
	QVBoxLayout *mainLayout;
	QHBoxLayout *topBar;
	QWidget     *topBarContainer = nullptr;
	QWidget     *tableContainer  = nullptr;

	void                 updateGlobalDateRange();
	void                 clearTopBarContainer();
	void                 clearHistoryPage();
	void                 populateTable(KakeiboTable *table);
	std::vector<Expense> collectFilteredExpenses();
	void                 fillTableWithExpenses(KakeiboTable *table, const std::vector<Expense> &expenses);

	void setUpTable(KakeiboTable *table);

	QHBoxLayout *createEnvelopeDropdown();
	QHBoxLayout *createDateDropdowns();
	QString      setUpComboStyleSheet();
	void         setupDateDropdownWidgets();
	void         initDateCombos();
	void         connectDateComboSignals();

	void handleRightClick(const QString &date, const QString &amount, const QString &envelope, const QString &desc);
};
