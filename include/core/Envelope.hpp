#pragma once

#include "ExpenseStruct.hpp"

#include <string>

class Envelope
{
  private:
	std::string              name;
	int                      amount;
	int                      dispatchAmount;
	int                      maxAmount;
	int                      goal;
	bool                     savings;
	std::vector<std::string> types;
	std::vector<Expense>     expenses;
	bool                     cloud;
	bool                     locked;

  public:
	Envelope()
	    : name(""), amount(0), dispatchAmount(0), maxAmount(0), goal(0), savings(false), types {}, cloud(false), locked(false) {}

	Envelope(const std::string &name, int amount, int maxAmount, int goal, bool savings)
	    : name(name), amount(amount), dispatchAmount(0), maxAmount(maxAmount), goal(goal), savings(savings), types {}, cloud(false), locked(false) {}

	Envelope(const Envelope &other)            = default;
	Envelope &operator=(const Envelope &other) = default;

	const std::string              &getName() const { return name; }
	int                             getAmount() const { return amount; }
	int                             getDispatchAmount() const { return dispatchAmount; }
	int                             getMaxAmount() const { return maxAmount; }
	int                             getGoal() const { return goal; }
	std::vector<std::string>       &getTypes() { return types; }
	const std::vector<std::string> &getTypes() const { return types; }
	bool                            isSavings() const { return savings; }
	const std::vector<Expense>     &getExpenses() const { return expenses; }
	std::vector<Expense>           &getExpensesMutable() { return expenses; }
	bool                            isCloud() const { return cloud; }
	bool                            isLocked() const { return locked; }

	void setName(const std::string &newName) { name = newName; }
	void setAmount(int newAmount) { amount = newAmount; }
	void setMaxAmount(int newMaxAmount) { maxAmount = newMaxAmount; }
	void setGoal(int newGoal) { goal = newGoal; }
	void setSavings(bool newSavings) { savings = newSavings; }
	void addType(const std::string &type) { types.push_back(type); }
	void addExpense(const int expense) { amount += expense; }
	void addToExpenseVector(const Expense &e) { expenses.push_back(e); }
	void setExpenses(const std::vector<Expense> &exps) { expenses = exps; }
	void setTypes(const std::vector<std::string> &t) { types = t; }
	void setCloud(const bool isCloud) { cloud = isCloud; }
	void setDispatchAmount(int val) { dispatchAmount = val; }
	void setLocked(bool l) { locked = l; }
};