#pragma once

#include <string>

class Enveloppe
{

private:
	std::string name;
	int amount;
	int maxAmount;
	int goal;
	bool savings;

public:

	Enveloppe(const std::string& name, int amount, int maxAmount, int goal, bool savings)
		: name(name), amount(amount), maxAmount(maxAmount), goal(goal), savings(savings) {}

	const std::string& getName() const { return name; }
	int getAmount() const { return amount; }
	int getMaxAmount() const { return maxAmount; }
	int getGoal() const { return goal; }
	bool isSavings() const { return savings; }

	void setName(const std::string& newName) { name = newName; }
	void setAmount(int newAmount) { amount = newAmount; }
	void setMaxAmount(int newMaxAmount) { maxAmount = newMaxAmount; }
	void setGoal(int newGoal) { goal = newGoal; }
	void setSavings(bool newSavings) { savings = newSavings; }
};