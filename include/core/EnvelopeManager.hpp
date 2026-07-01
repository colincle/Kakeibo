#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Envelope.hpp"

class QString;

class EnvelopeManager
{
  public:
	EnvelopeManager();
	EnvelopeManager(const EnvelopeManager &other);
	EnvelopeManager &operator=(const EnvelopeManager &other);

	std::vector<Envelope> &getEnvelopes() { return envelopes; }
	Envelope              &getIncomeEnvelope() { return incomeEnvelope; }
	Envelope              &getCreditEnvelope() { return creditEnvelope; }

	void addEnvelope(const std::string &name, int amount, int maxAmount, int goal, bool savings);
	void modifyEnvelope(const std::string &oldName, const std::string &name, int amount, int maxAmount, int goal, bool savings);
	void transfer(std::string from, std::string to, int amount);
	void addTypeAndExpense(const std::string &name, const Expense &e, bool rememberType);
	void addExpense(Expense e, Envelope &env);
	void moveEnvelope(const std::string &name, bool up);
	void deleteEnvelope(const std::string &name);
	void switchCloud(const std::string &name);
	void moveExpenseToNewEnvelope(const QString &date, const QString &amount, const QString &srcEnv, const QString &desc, const QString &destEnv);
	void forgetExpenseType(const QString &envelope, const QString &desc);
	void deleteExpense(const QString &date, const QString &amount, const QString &envelope, const QString &desc);
	void saveEnvelopesToJson();

	std::filesystem::path getPath() { return basePath; }

  private:
	std::vector<Envelope> envelopes;
	Envelope              creditEnvelope;
	Envelope              incomeEnvelope;
	std::filesystem::path basePath;
	std::filesystem::path envelopesPath;
	std::filesystem::path specialEnvelopesPath;

	void     createPaths();
	void     getEnvelopesFromJson();
	Envelope importEnvelope(const json &item);
	json     exportEnvelope(const Envelope &env);
};