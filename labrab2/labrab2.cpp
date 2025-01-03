﻿/******************************************************************************

							  Online C++ Compiler.
			   Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

const int SPEC_TYPES_AMOUNT = 3;
enum specTypes { Engieneer, Miner, Transporter };

int specAmount;
float* specDayPay;
specTypes* types;

float budget;

const float specBonus = 101;
const float brigadeBonus = 1000.1;

struct Brigade {
	int counts[SPEC_TYPES_AMOUNT];

	float getBrigadePayoff() {
		int commonCounts = 0;
		for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
			commonCounts += counts[i];
		}

		return commonCounts * specBonus + brigadeBonus;
	}
	std::string toString() {
		std::string str = "{ ";
		for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
			str += std::to_string(counts[i]) + " ";
		}
		str += "}";
		return str;
	}
};

const int BRIGADES_AMOUNT = 3; // >= 3
Brigade brigades[BRIGADES_AMOUNT] = {
	{{2, 3, 2}},
	{{4, 1, 2}},
	{{1, 2, 4}}
};


std::vector<int> reservation[SPEC_TYPES_AMOUNT];

// @return cost for paying specs, 0 if not possible
float reserveSpecs(Brigade brigade) {
	float cost = 0.0;
	int counts[SPEC_TYPES_AMOUNT]; // сколько зарезервировали

	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		counts[i] = 0;

		std::vector<std::pair<float, int>> specialists;
		for (int j = 0; j < specAmount; j++) {
			if (
				types[j] == i &&                                                                    // if types match AND
				std::find(reservation[i].begin(), reservation[i].end(), j) == reservation[i].end() // if j not found in reservation[i]
				) {
				specialists.push_back({ specDayPay[j], j });
			}
		}

		if (specialists.size() < brigade.counts[i]) {
			for (int j = 0; j < SPEC_TYPES_AMOUNT; j++) {
				for (int k = counts[j]; k > 0; k--) {
					reservation[j].pop_back();
				}
			}

			return 0;
		}

		std::sort(specialists.begin(), specialists.end()); // сортировка по 1-у элементу пары

		int count = brigade.counts[i];
		for (const auto& specialist : specialists) {
			if (count-- <= 0) {
				break;
			}

			cost += specialist.first;
			reservation[i].push_back(specialist.second);
			counts[i]++;
		}
	}

	return cost;
}


void unreserveSpecs(Brigade brigade) {
	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		for (int j = brigade.counts[i]; j > 0; j--) {
			reservation[i].pop_back();
		}
	}
}


int getLeftoverSpecsAmountForBudget(
	float budget, 
	std::vector<int> reservation[SPEC_TYPES_AMOUNT], 
	bool out = false
) {
	float remainingBudget = budget;
	int amount = 0;

	if (out)
		std::cout << "Add-on workers - ";

	std::vector<std::pair<float, int>> specialists;

	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		for (int j = 0; j < specAmount; j++) {
			if (
				types[j] == i &&                                                                    // if types match AND
				std::find(reservation[i].begin(), reservation[i].end(), j) == reservation[i].end() // if j not found in reservation[i]
				) {
				specialists.push_back({ specDayPay[j], j });
			}
		}
	}

	std::sort(specialists.begin(), specialists.end()); // сортировка по 1-у элементу пары

	for (const auto& specialist : specialists) {
		remainingBudget -= specialist.first;

		if (remainingBudget >= 0) {
			amount++;
			if (out) 
				std::cout << specialist.second << "(" << specialist.first << ") ";
		}
		else {
			break;
		}
	}

	if (out) {
		float cost = 0.0;
		for (int i = 0; i < amount; i++)
			cost += specialists[i].first;

		std::cout << "; \n\t" << specBonus << " per spec = "
			<< specBonus << " * " << amount << " = "
			<< specBonus * amount << std::endl
			<< "\tcost = " << cost << ", remaining budget = " << budget - cost
			<< std::endl << std::endl;

		if (amount < specialists.size())
			std::cout << "Workers unemployed: ";
		else
			std::cout << "Everyone employed." << std::endl;

		for (int i = amount; i < specialists.size(); i++) {
			std::cout << specialists[i].second << "(" 
				<< specialists[i].first << ") ";
		}

		if (amount < specialists.size())
			std::cout << "; \n\t" 
				<< "budget diff for next worker = " << remainingBudget
				<< std::endl << std::endl;
	}

	return amount;
}


float gPayoff = 0;
float gRemainingBudget = budget;
std::vector<int> bestReservation[SPEC_TYPES_AMOUNT];
std::vector<Brigade> gBrigades;
std::vector<Brigade> bestBrigades;

float getMaxPossiblePayoff(float remainingBudget, float payoffDownwards = 0.0) {
	float payoff = 0.0;

	payoff = getLeftoverSpecsAmountForBudget(remainingBudget, reservation) * specBonus;

	if (payoffDownwards + payoff > gPayoff) {
		gPayoff = payoffDownwards + payoff;
		gRemainingBudget = remainingBudget;
		bestBrigades = gBrigades;

		for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
			bestReservation[i] = reservation[i];
		}
	}


	for (int i = 0; i < BRIGADES_AMOUNT; i++) {
		float localPayoff = 0.0;
		float resBudget = reserveSpecs(brigades[i]);

		if (resBudget > 0 && remainingBudget - resBudget >= 0) {
			gBrigades.push_back(brigades[i]);

			localPayoff = brigades[i].getBrigadePayoff()
				+ getMaxPossiblePayoff(	remainingBudget - resBudget, 
										payoffDownwards + brigades[i].getBrigadePayoff());

			gBrigades.pop_back();

			if (localPayoff > payoff) {
				payoff = localPayoff;
			}
		}

		if (resBudget > 0) unreserveSpecs(brigades[i]);
	}

	return payoff;
}


int inputFromFile(std::string file) {
	// ---------------- Ввод --------------------
	std::ifstream inputFile(file);
	if (!inputFile) {
		std::cerr << "Не удалось открыть файл!" << std::endl;
		return 1;
	}

	inputFile >> specAmount >> budget;

	specDayPay = new float[specAmount];
	types = new specTypes[specAmount];

	for (int i = 0; i < specAmount; ++i) {
		inputFile >> specDayPay[i];
	}

	std::string type;
	for (int i = 0; i < specAmount; ++i) {
		inputFile >> type;
		if (type == "Engieneer") {
			types[i] = Engieneer;
		}
		else if (type == "Miner") {
			types[i] = Miner;
		}
		else if (type == "Transporter") {
			types[i] = Transporter;
		}
		else {
			std::cerr << "Неизвестный тип: " << type << std::endl;
			return 1;
		}
	}

	inputFile.close();
	//-------------------------------------------

	return 0;
}
void outputStatsInConsole() {
	float remainingBudget = budget;
	std::cout << "Budget = " << remainingBudget << std::endl << std::endl;
	std::cout << "Brigades: ";
	for (Brigade brigade : bestBrigades) {
		std::cout << brigade.toString() << " ";
	}
	std::cout << "; \n\t" << brigadeBonus << " per brigade = "
		<< brigadeBonus << " * " << bestBrigades.size() << " = "
		<< brigadeBonus * bestBrigades.size() << std::endl << std::endl;
	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		float cost = 0.0;

		if (i == 0) std::cout << "Engieneers   - ";
		if (i == 1) std::cout << "Miners       - ";
		if (i == 2) std::cout << "Transporters - ";

		for (int j : bestReservation[i]) {
			std::cout << j << "(" << specDayPay[j] << ") ";
			cost += specDayPay[j];
		}

		remainingBudget = remainingBudget - cost;
		std::cout << "; \n\t" << specBonus << " per spec = "
			<< specBonus << " * " << bestReservation[i].size() << " = "
			<< specBonus * bestReservation[i].size() << std::endl
			<< "\tcost = " << cost << ", remaining budget = " << remainingBudget
			<< std::endl << std::endl;
	}
	getLeftoverSpecsAmountForBudget(gRemainingBudget, bestReservation, true);
}


int main()
{
	if (inputFromFile("input.txt"))
		return 1;


	std::cout << "Max payoff = " << getMaxPossiblePayoff(budget) << std::endl;
	outputStatsInConsole();


	delete[] specDayPay;
	delete[] types;

	return 0;
}
