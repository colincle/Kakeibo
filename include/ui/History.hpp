#pragma once

#include <QString>
#include <QWidget>
#include <chrono>
#include <string>
#include <vector>

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QScrollArea;
class QTableWidget;
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

	QComboBox   *enveloppeCombo;
	QComboBox   *startCombo;
	QComboBox   *endCombo;
	QVBoxLayout *mainLayout;
	QHBoxLayout *topBar;
	QWidget     *topBarContainer = nullptr;
	QScrollArea *scrollArea      = nullptr;
	QWidget     *tableContainer  = nullptr;

	void                 updateGlobalDateRange();
	void                 clearTopBarContainer();
	void                 clearHistoryPage();
	void                 populateTable(QTableWidget *table);
	std::vector<Expense> collectFilteredExpenses();
	void                 fillTableWithExpenses(QTableWidget *table, const std::vector<Expense> &expenses);

	void    setUpTable(QTableWidget *table);
	QString setTableStyleSheet();

	QHBoxLayout *createEnveloppeDropdown();
	QHBoxLayout *createDateDropdowns();
	QString      setUpComboStyleSheet();
	void         setupDateDropdownWidgets();
	void         initDateCombos();
	void         connectDateComboSignals();

	void handleRightClick(const QString &date, const QString &amount, const QString &enveloppe, const QString &desc);
};
