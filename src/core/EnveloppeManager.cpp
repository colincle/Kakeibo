#include "EnveloppeManager.hpp"
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;

EnveloppeManager::EnveloppeManager()
{
	createPaths();
	getEnveloppesFromJson();
}

void EnveloppeManager::createPaths()
{
	const char* xdg = std::getenv("XDG_DATA_HOME");
	std::filesystem::path base = xdg
	                             ? std::filesystem::path(xdg)
	                             : std::filesystem::path(std::getenv("HOME")) / ".local" / "share";
	basePath = base / "kakeibo";
	enveloppesPath = basePath / "enveloppes.json";
	std::filesystem::create_directories(basePath);
}

void EnveloppeManager::getEnveloppesFromJson()
{
	if(!std::filesystem::exists(enveloppesPath))
	{
		std::ofstream file(enveloppesPath);
		json empty = json::object();
		file << empty.dump(4);
		return ;
	}

	std::ifstream file(enveloppesPath);
	json data = json::parse(file);

	for(const auto& item : data)
	{
		enveloppes.emplace_back(
		    item.at("name").get<std::string>(),
		    item.at("amount").get<int>(),
		    item.at("maxAmount").get<int>(),
		    item.at("goal").get<int>(),
		    item.at("savings").get<bool>()
		);
	}
}

void EnveloppeManager::saveEnveloppesToJson()
{
	json data = json::array();

	for(const auto& env : enveloppes)
	{
		data.push_back(
		{
			{ "name", env.getName() },
			{ "amount", env.getAmount() },
			{ "maxAmount", env.getMaxAmount() },
			{ "goal", env.getGoal() },
			{ "savings", env.isSavings() }
		});
	}

	std::ofstream file(enveloppesPath);
	file << data.dump(4);
}

void EnveloppeManager::addEnveloppe(const std::string& name, int amount, int maxAmount, int goal, bool savings)
{
	enveloppes.emplace_back(name, amount, maxAmount, goal, savings);
	saveEnveloppesToJson();
}

void EnveloppeManager::transfer(std::string from, std::string to, int amount)
{
	int found = 0;

	for(Enveloppe& e : enveloppes)
	{
		if(e.getName() == from)
		{
			e.setAmount(e.getAmount() - amount);

			if(++found == 2)
				break;
		}
		else
			if(e.getName() == to)
			{
				e.setAmount(e.getAmount() + amount);

				if(++found == 2)
					break;
			}
	}

	saveEnveloppesToJson();
}
