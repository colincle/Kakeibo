#include "EnveloppeManager.hpp"
#include "json.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>

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
	std::filesystem::create_directories(basePath);
}

void EnveloppeManager::getEnveloppesFromJson()
{
	if ( !std::filesystem::exists(enveloppesPath) )
	{
		std::ofstream file(enveloppesPath);
		json          empty = json::object();
		file << empty.dump(4);
		return;
	}

	std::ifstream file(enveloppesPath);
	json          data = json::parse(file);

	for ( const auto &item : data )
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

		enveloppes.push_back(std::move(e));
	}
}

void EnveloppeManager::saveEnveloppesToJson()
{
	json data = json::array();

	for ( const auto &env : enveloppes )
	{
		data.push_back(
		    {{"name", env.getName()},
		     {"amount", env.getAmount()},
		     {"maxAmount", env.getMaxAmount()},
		     {"goal", env.getGoal()},
		     {"savings", env.isSavings()},
		     {"types", env.getTypes()},
		     {"expenses", env.getExpenses()}});
	}

	std::ofstream file(enveloppesPath);
	file << data.dump(4);
}

void EnveloppeManager::addEnveloppe(const std::string &name, int amount, int maxAmount, int goal, bool savings)
{
	enveloppes.emplace_back(name, amount, maxAmount, goal, savings);
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

void EnveloppeManager::addTypeAndExpense(const std::string &name, const Expense &e)
{
	for ( auto &env : getEnveloppes() )
		if ( env.getName() == name )
		{
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
