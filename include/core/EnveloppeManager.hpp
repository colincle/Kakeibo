#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "Enveloppe.hpp"

class EnveloppeManager
{
public:

	EnveloppeManager();

	std::vector<Enveloppe> getEnveloppes() { return enveloppes; }

	void addEnveloppe(const std::string& name, int amount, int maxAmount, int goal, bool savings);
	void transfer(std::string from, std::string to, int amount);

private:

	std::vector<Enveloppe> enveloppes;
	std::filesystem::path basePath;
	std::filesystem::path enveloppesPath;

	void createPaths();
	void getEnveloppesFromJson();
	void saveEnveloppesToJson();
};