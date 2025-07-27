#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "Enveloppe.hpp"

class EnveloppeManager
{
public:

	EnveloppeManager();

	std::vector<Enveloppe>& getEnveloppes() { return enveloppes; }

	void addEnveloppe(const std::string& name, int amount, int maxAmount, int goal, bool savings);
	void transfer(std::string from, std::string to, int amount);
	void addTypeAndExpense(const std::string& name, const Expense& e);
	void addExpense(Expense e, Enveloppe& env);
	void moveEnveloppe(const std::string& name, bool up);
	void deleteEnveloppe(const std::string& name);

private:

	std::vector<Enveloppe> enveloppes;
	std::filesystem::path basePath;
	std::filesystem::path enveloppesPath;

	void createPaths();
	void getEnveloppesFromJson();
	void saveEnveloppesToJson();
};