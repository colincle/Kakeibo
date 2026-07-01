#include "EnvelopeManager.hpp"
#include "Globals.hpp"
#include "json.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>

#include <QString>

using json = nlohmann::json;

EnvelopeManager::EnvelopeManager()
{
	createPaths();
	getEnvelopesFromJson();
}

EnvelopeManager::EnvelopeManager(const EnvelopeManager &other)
    : envelopes(other.envelopes),
      creditEnvelope(other.creditEnvelope),
      incomeEnvelope(other.incomeEnvelope),
      basePath(other.basePath),
      envelopesPath(other.envelopesPath),
      specialEnvelopesPath(other.specialEnvelopesPath)
{
}

EnvelopeManager &EnvelopeManager::operator=(const EnvelopeManager &other)
{
	if ( this != &other )
	{
		envelopes            = other.envelopes;
		creditEnvelope       = other.creditEnvelope;
		incomeEnvelope       = other.incomeEnvelope;
		basePath             = other.basePath;
		envelopesPath        = other.envelopesPath;
		specialEnvelopesPath = other.specialEnvelopesPath;
	}

	return *this;
}

void EnvelopeManager::createPaths()
{
	const char *xdg  = std::getenv("XDG_DATA_HOME");
	const char *home = std::getenv("HOME");

	std::filesystem::path base;

	if ( xdg && *xdg )
		base = std::filesystem::path(xdg);
	else if ( home && *home )
		base = std::filesystem::path(home) / ".local" / "share";
	else
		base = std::filesystem::current_path();

	basePath             = base / "kakeibo";
	envelopesPath        = basePath / "enveloppes.json";
	specialEnvelopesPath = basePath / "specialEnveloppes.json";
	std::filesystem::create_directories(basePath);
}

void EnvelopeManager::getEnvelopesFromJson()
{
	if ( !std::filesystem::exists(envelopesPath) )
	{
		std::ofstream file(envelopesPath);
		json          empty = json::object();
		file << empty.dump(4);
	}
	else
	{
		std::ifstream file(envelopesPath);

		try
		{
			json data = json::parse(file);

			for ( const auto &item : data )
			{
				Envelope e = importEnvelope(item);
				envelopes.push_back(std::move(e));
			}
		}
		catch ( const std::exception &err )
		{
			std::fprintf(stderr, "Failed to read %s: %s\n", envelopesPath.string().c_str(), err.what());
		}
	}

	json special;
	bool changed = false;

	if ( std::filesystem::exists(specialEnvelopesPath) )
	{
		std::ifstream specialFile(specialEnvelopesPath);

		try
		{
			special = json::parse(specialFile);
		}
		catch ( const std::exception &err )
		{
			std::fprintf(stderr, "Failed to read %s: %s\n", specialEnvelopesPath.string().c_str(), err.what());
		}
	}

	if ( !special.contains("Income") )
	{
		special["Income"] =
		    {
		        {"name", "Revenus\n収入"},
		        {"amount", 0},
		        {"maxAmount", 0},
		        {"goal", 0},
		        {"savings", false}};
		changed = true;
	}

	if ( !special.contains("Credit") )
	{
		special["Credit"] =
		    {
		        {"name", "Crédit\nクレジット"},
		        {"amount", 0},
		        {"maxAmount", 0},
		        {"goal", 0},
		        {"savings", false}};
		changed = true;
	}

	incomeEnvelope = importEnvelope(special["Income"]);
	creditEnvelope = importEnvelope(special["Credit"]);

	if ( changed )
	{
		std::ofstream outFile(specialEnvelopesPath);
		outFile << special.dump(4);
	}
}

Envelope EnvelopeManager::importEnvelope(const json &item)
{
	Envelope e(
	    item.at("name").get<std::string>(),
	    item.at("amount").get<int>(),
	    item.at("maxAmount").get<int>(),
	    item.at("goal").get<int>(),
	    item.at("savings").get<bool>());

	if ( item.contains("types") )
		e.setTypes(item.at("types").get<std::vector<std::string>>());

	if ( item.contains("expenses") )
		e.setExpenses(item.at("expenses").get<std::vector<Expense>>());

	if ( item.contains("cloud") )
		e.setCloud(item.at("cloud").get<bool>());
	return e;
}

void EnvelopeManager::saveEnvelopesToJson()
{
	json data = json::array();

	for ( const auto &env : envelopes )
		data.push_back(exportEnvelope(env));

	std::ofstream file(envelopesPath);
	file << data.dump(4);

	json special;
	special["Income"] = exportEnvelope(incomeEnvelope);
	special["Credit"] = exportEnvelope(creditEnvelope);

	std::ofstream specialFile(specialEnvelopesPath);
	specialFile << special.dump(4);
}

json EnvelopeManager::exportEnvelope(const Envelope &env)
{
	return {
	    {"name", env.getName()},
	    {"amount", env.getAmount()},
	    {"maxAmount", env.getMaxAmount()},
	    {"goal", env.getGoal()},
	    {"savings", env.isSavings()},
	    {"types", env.getTypes()},
	    {"expenses", env.getExpenses()},
	    {"cloud", env.isCloud()}};
}

void EnvelopeManager::addEnvelope(const std::string &name, int amount, int maxAmount, int goal, bool savings)
{
	Envelope env(name, amount, maxAmount, goal, savings);
	env.setCloud(false);
	envelopes.push_back(std::move(env));
	saveEnvelopesToJson();
}

void EnvelopeManager::modifyEnvelope(const std::string &oldName, const std::string &name, int amount, int maxAmount, int goal, bool savings)
{
	for ( auto &env : envelopes )
	{
		if ( env.getName() == oldName )
		{
			env.setName(name);
			env.setAmount(amount);
			env.setMaxAmount(maxAmount);
			env.setGoal(goal);
			env.setSavings(savings);
			break;
		}
	}

	saveEnvelopesToJson();
}

void EnvelopeManager::transfer(std::string from, std::string to, int amount)
{
	auto apply = [&](Envelope &e, const std::string &name, int delta)
	{
		if ( e.getName() == name )
		{
			e.setAmount(e.getAmount() + delta);
			return true;
		}

		return false;
	};

	int found = 0;

	for ( Envelope &e : envelopes )
	{
		if ( apply(e, from, -amount) || apply(e, to, amount) )
			if ( ++found == 2 )
				break;
	}

	if ( found < 2 )
	{
		if ( apply(creditEnvelope, from, -amount) || apply(creditEnvelope, to, amount) )
			++found;

		if ( apply(incomeEnvelope, from, -amount) || apply(incomeEnvelope, to, amount) )
			++found;
	}

	saveEnvelopesToJson();
}

void EnvelopeManager::addTypeAndExpense(const std::string &name, const Expense &e, bool rememberType)
{
	if ( g_envelopeManager.getCreditEnvelope().getName() == name )
	{
		if ( rememberType )
			g_envelopeManager.getCreditEnvelope().addType(e.info);

		addExpense(e, g_envelopeManager.getCreditEnvelope());
		return;
	}

	for ( auto &env : getEnvelopes() )
		if ( env.getName() == name )
		{
			if ( rememberType )
				env.addType(e.info);

			addExpense(e, env);
		}
}

void EnvelopeManager::addExpense(Expense e, Envelope &env)
{
	if ( e.isCredit )
		creditEnvelope.addExpense(-e.amount);

	e.envelope = env.getName();
	env.addExpense(e.amount);
	env.addToExpenseVector(e);
	saveEnvelopesToJson();
}

void EnvelopeManager::switchCloud(const std::string &name)
{
	for ( auto &env : getEnvelopes() )
	{
		if ( env.getName() == name )
		{
			env.setCloud(!env.isCloud());
			saveEnvelopesToJson();
			break;
		}
	}
}

void EnvelopeManager::moveEnvelope(const std::string &name, bool up)
{
	for ( size_t i = 0; i < envelopes.size(); ++i )
	{
		if ( envelopes[i].getName() == name )
		{
			if ( up && i > 0 )
				std::swap(envelopes[i], envelopes[i - 1]);
			else if ( !up && i + 1 < envelopes.size() )
				std::swap(envelopes[i], envelopes[i + 1]);

			break;
		}
	}

	saveEnvelopesToJson();
}

void EnvelopeManager::deleteEnvelope(const std::string &name)
{
	auto it = std::remove_if(envelopes.begin(), envelopes.end(), [&](const Envelope &e)
	                         { return e.getName() == name; });

	if ( it != envelopes.end() )
	{
		envelopes.erase(it, envelopes.end());
		saveEnvelopesToJson();
	}
}

void EnvelopeManager::moveExpenseToNewEnvelope(const QString &date, const QString &amount, const QString &srcEnv, const QString &desc, const QString &destEnv)
{
	Expense            expense = {};
	std::istringstream iss(date.toStdString());
	int                y, m, d;
	char               sep1, sep2;
	iss >> y >> sep1 >> m >> sep2 >> d;
	expense.date     = std::chrono::year {y} / std::chrono::month {static_cast<unsigned int>(m)} / std::chrono::day {static_cast<unsigned int>(d)};
	expense.amount   = amount.toInt();
	expense.info     = desc.toStdString();
	expense.envelope = destEnv.toStdString();

	addTypeAndExpense(destEnv.toStdString(), expense, false);
	deleteExpense(date, amount, srcEnv, desc);
}

void EnvelopeManager::forgetExpenseType(const QString &envelope, const QString &desc)
{
	for ( auto &env : envelopes )
	{
		if ( QString::fromStdString(env.getName()) != envelope )
			continue;

		auto &types = env.getTypes();
		auto  it    = std::find(types.begin(), types.end(), desc.toStdString());

		if ( it != types.end() )
		{
			types.erase(it);
			saveEnvelopesToJson();
		}

		break;
	}
}

void EnvelopeManager::deleteExpense(const QString &date, const QString &amount, const QString &envelope, const QString &desc)
{
	for ( auto &env : envelopes )
	{
		if ( QString::fromStdString(env.getName()) != envelope )
			continue;

		std::string target   = (date + "|" + amount + "|" + desc).toStdString();
		auto       &expenses = env.getExpensesMutable();

		auto it = std::find_if(expenses.begin(), expenses.end(), [&](const Expense &e)
		                       {
			std::string current = std::format("{}|{}|{}", e.date, e.amount, e.info);
			return current == target; });

		if ( it != expenses.end() )
		{
			expenses.erase(it);
			env.setAmount(env.getAmount() - amount.toInt());
			saveEnvelopesToJson();
			forgetExpenseType(envelope, desc);
			return;
		}
	}
}
