#include "EnveloppeManager.hpp"
#include "json.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <format>

#include <QString>

using json = nlohmann::json;

EnveloppeManager::EnveloppeManager()
{
	createPaths();
	getEnveloppesFromJson();
}

void EnveloppeManager::createPaths()
{
	const char           *xdg  = std::getenv("XDG_DATA_HOME");
	std::filesystem::path base = xdg
	                                 ? std::filesystem::path(xdg)
	                                 : std::filesystem::path(std::getenv("HOME")) / ".local" / "share";
	basePath                   = base / "kakeibo";
	enveloppesPath             = basePath / "enveloppes.json";
	specialEnveloppesPath             = basePath / "specialEnveloppes.json";
	std::filesystem::create_directories(basePath);
}

void EnveloppeManager::getEnveloppesFromJson()
{
	if (!std::filesystem::exists(enveloppesPath))
	{
		std::ofstream file(enveloppesPath);
		json empty = json::object();
		file << empty.dump(4);
	}
	else
	{
		std::ifstream file(enveloppesPath);
		json data = json::parse(file);

		for (const auto &item : data)
		{
			Enveloppe e = importEnveloppe(item);
			enveloppes.push_back(std::move(e));
		}
	}

	json special;
	bool changed = false;

	if (std::filesystem::exists(specialEnveloppesPath))
	{
		std::ifstream specialFile(specialEnveloppesPath);
		special = json::parse(specialFile);
	}

	if (!special.contains("Income"))
	{
		special["Income"] = {
			{"name", "Revenus\n収入"},
			{"amount", 0},
			{"maxAmount", 0},
			{"goal", 0},
			{"savings", false}
		};
		changed = true;
	}
	if (!special.contains("Credit"))
	{
		special["Credit"] = {
			{"name", "Crédit\nクレジット"},
			{"amount", 0},
			{"maxAmount", 0},
			{"goal", 0},
			{"savings", false}
		};
		changed = true;
	}

	incomeEnveloppe = importEnveloppe(special["Income"]);
	creditEnveloppe = importEnveloppe(special["Credit"]);

	if (changed)
	{
		std::ofstream outFile(specialEnveloppesPath);
		outFile << special.dump(4);
	}
}


Enveloppe EnveloppeManager::importEnveloppe(const json &item)
{
	Enveloppe e(
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

void EnveloppeManager::saveEnveloppesToJson()
{
	json data = json::array();

	for (const auto &env : enveloppes)
		data.push_back(exportEnveloppe(env));

	std::ofstream file(enveloppesPath);
	file << data.dump(4);

	json special;
	special["Income"] = exportEnveloppe(incomeEnveloppe);
	special["Credit"] = exportEnveloppe(creditEnveloppe);

	std::ofstream specialFile(specialEnveloppesPath);
	specialFile << special.dump(4);
}

json EnveloppeManager::exportEnveloppe(const Enveloppe &env)
{
	return {
		{"name", env.getName()},
		{"amount", env.getAmount()},
		{"maxAmount", env.getMaxAmount()},
		{"goal", env.getGoal()},
		{"savings", env.isSavings()},
		{"types", env.getTypes()},
		{"expenses", env.getExpenses()},
		{"cloud", env.isCloud()}
	};
}


void EnveloppeManager::addEnveloppe(const std::string &name, int amount, int maxAmount, int goal, bool savings)
{
	Enveloppe env(name, amount, maxAmount, goal, savings);
	env.setCloud(false);
	enveloppes.push_back(std::move(env));
	saveEnveloppesToJson();
}

void EnveloppeManager::modifyEnveloppe(const std::string &oldName, const std::string &name, int amount, int maxAmount, int goal, bool savings)
{
	for ( auto &env : enveloppes )
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

	saveEnveloppesToJson();
}

void EnveloppeManager::transfer(std::string from, std::string to, int amount)
{
	int found = 0;

	for ( Enveloppe &e : enveloppes )
	{
		if ( e.getName() == from )
		{
			e.setAmount(e.getAmount() - amount);

			if ( ++found == 2 )
				break;
		}
		else if ( e.getName() == to )
		{
			e.setAmount(e.getAmount() + amount);

			if ( ++found == 2 )
				break;
		}
	}

	saveEnveloppesToJson();
}

void EnveloppeManager::addTypeAndExpense(const std::string &name, const Expense &e, bool rememberType)
{
	for ( auto &env : getEnveloppes() )
		if ( env.getName() == name )
		{
			if ( rememberType )
				env.addType(e.info);

			addExpense(e, env);
		}
}

void EnveloppeManager::addExpense(Expense e, Enveloppe &env)
{
	e.enveloppe = env.getName();
	env.addExpense(e.amount);
	env.addToExpenseVector(e);
	saveEnveloppesToJson();
}

void EnveloppeManager::switchCloud(const std::string &name)
{
	for ( auto &env : getEnveloppes() )
	{
		if ( env.getName() == name )
		{
			env.setCloud(!env.isCloud());
			saveEnveloppesToJson();
			break;
		}
	}
}

void EnveloppeManager::moveEnveloppe(const std::string &name, bool up)
{
	for ( size_t i = 0; i < enveloppes.size(); ++i )
	{
		if ( enveloppes[i].getName() == name )
		{
			if ( up && i > 0 )
				std::swap(enveloppes[i], enveloppes[i - 1]);
			else if ( !up && i + 1 < enveloppes.size() )
				std::swap(enveloppes[i], enveloppes[i + 1]);

			break;
		}
	}

	saveEnveloppesToJson();
}

void EnveloppeManager::deleteEnveloppe(const std::string &name)
{
	auto it = std::remove_if(enveloppes.begin(), enveloppes.end(), [&](const Enveloppe &e)
	                         { return e.getName() == name; });

	if ( it != enveloppes.end() )
	{
		enveloppes.erase(it, enveloppes.end());
		saveEnveloppesToJson();
	}
}

void EnveloppeManager::moveExpenseToNewEnveloppe(const QString &date, const QString &amount, const QString &srcEnv, const QString &desc, const QString &destEnv)
{
	Expense expense = {};
	std::istringstream iss(date.toStdString());
	int y, m, d;
	char sep1, sep2;
	iss >> y >> sep1 >> m >> sep2 >> d;
	expense.date = std::chrono::year{y} / std::chrono::month{static_cast<unsigned int>(m)} / std::chrono::day{static_cast<unsigned int>(d)};
	expense.amount = amount.toInt();
	expense.info = desc.toStdString();
	expense.enveloppe = destEnv.toStdString();

	addTypeAndExpense(destEnv.toStdString(), expense, false);
	deleteExpense(date, amount, srcEnv, desc);
}

void EnveloppeManager::forgetExpenseType(const QString &enveloppe, const QString &desc)
{
	for (auto &env : enveloppes)
	{
		if (QString::fromStdString(env.getName()) != enveloppe)
			continue;

		auto &types = env.getTypes();
		auto it = std::find(types.begin(), types.end(), desc.toStdString());

		if (it != types.end())
		{
			types.erase(it);
			saveEnveloppesToJson();
		}

		break;
	}
}

void EnveloppeManager::deleteExpense(const QString &date, const QString &amount, const QString &enveloppe, const QString &desc)
{
	for (auto &env : enveloppes)
	{
		if (QString::fromStdString(env.getName()) != enveloppe)
			continue;

		std::string target = (date + "|" + amount + "|" + desc).toStdString();
		auto &expenses = env.getExpensesMutable();

		auto it = std::find_if(expenses.begin(), expenses.end(), [&](const Expense &e) {
			std::string current = std::format("{}|{}|{}", e.date, e.amount, e.info);
			return current == target;
		});

		if (it != expenses.end())
		{
			expenses.erase(it);
			env.setAmount(env.getAmount() - amount.toInt());
			saveEnveloppesToJson();
			forgetExpenseType(enveloppe, desc);
			return;
		}
	}
}
