#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Enveloppe.hpp"

class QString;

class EnveloppeManager
{
  public:
	EnveloppeManager();

	std::vector<Enveloppe> &getEnveloppes() { return enveloppes; }

	void                  addEnveloppe(const std::string &name, int amount, int maxAmount, int goal, bool savings);
	void                  modifyEnveloppe(const std::string &oldName, const std::string &name, int amount, int maxAmount, int goal, bool savings);
	void                  transfer(std::string from, std::string to, int amount);
	void                  addTypeAndExpense(const std::string &name, const Expense &e, bool rememberType);
	void                  addExpense(Expense e, Enveloppe &env);
	void                  moveEnveloppe(const std::string &name, bool up);
	void                  deleteEnveloppe(const std::string &name);
	void                  switchCloud(const std::string &name);
	void moveExpenseToNewEnveloppe(const QString &date, const QString &amount, const QString &srcEnv, const QString &desc, const QString &destEnv);
	void forgetExpenseType(const QString &enveloppe, const QString &desc);
	void deleteExpense(const QString &date, const QString &amount, const QString &enveloppe, const QString &desc);
	
	std::filesystem::path getPath() { return basePath; }

  private:
	std::vector<Enveloppe> enveloppes;
	std::filesystem::path  basePath;
	std::filesystem::path  enveloppesPath;

	void createPaths();
	void getEnveloppesFromJson();
	void saveEnveloppesToJson();
};