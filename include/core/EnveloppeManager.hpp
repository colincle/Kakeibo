#pragma once
#include <string>
#include <Enveloppe.hpp>
#include <filesystem>
#include <vector>

class EnveloppeManager
{
public:
	std::vector<Enveloppe> enveloppes;

	EnveloppeManager();
	void addEnveloppe(const std::string& name, int amount, int maxAmount, int goal, bool savings);

private:
	std::filesystem::path basePath;
	std::filesystem::path enveloppesPath;

	void createPaths();
	void getEnveloppesFromJson();
	void saveEnveloppesToJson();
};