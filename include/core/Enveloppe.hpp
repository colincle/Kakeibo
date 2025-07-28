#pragma once

#include <ExpenseStruct.hpp>

#include <string>

class Enveloppe
{
  private:
	std::string              name;
	int                      amount;
	int                      maxAmount;
	int                      goal;
	bool                     savings;
	std::vector<std::string> types;
	std::vector<Expense>     expenses;
	bool                     cloud;

  public:
	Enveloppe(const std::string &name, int amount, int maxAmount, int goal, bool savings)
	    : name(name), amount(amount), maxAmount(maxAmount), goal(goal), savings(savings), types {} {}

	const std::string          &getName() const { return name; }
	int                         getAmount() const { return amount; }
	int                         getMaxAmount() const { return maxAmount; }
	int                         getGoal() const { return goal; }
	std::vector<std::string>& getTypes() { return types; }
	const std::vector<std::string>& getTypes() const { return types; }
	bool                        isSavings() const { return savings; }
	const std::vector<Expense> &getExpenses() const { return expenses; }
	std::vector<Expense> &getExpensesMutable() { return expenses; }
	bool                        isCloud() const { return cloud; }

	void setName(const std::string &newName) { name = newName; }
	void setAmount(int newAmount) { amount = newAmount; }
	void setMaxAmount(int newMaxAmount) { maxAmount = newMaxAmount; }
	void setGoal(int newGoal) { goal = newGoal; }
	void setSavings(bool newSavings) { savings = newSavings; }
	void addType(const std::string &type) { types.push_back(type); }
	void addExpense(const int expense) { amount += expense; }
	void addToExpenseVector(const Expense e) { expenses.push_back(e); }
	void setExpenses(const std::vector<Expense> &exps) { expenses = exps; }
	void setTypes(const std::vector<std::string> &t) { types = t; }
	void setCloud(const bool isCloud) { cloud = isCloud; }
};