#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

const int SPEC_TYPES_AMOUNT = 3;
enum specTypes { Engieneer, Miner, Transporter };

int specAmount;
float* specDayPay;
specTypes* types;

const float budget = 3200; //3600;

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
		budget -= specialist.first;

		if (budget >= 0) {
			amount++;
			if (out) std::cout << specialist.second << " ";
		}
		else {
			break;
		}
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
				+ getMaxPossiblePayoff(remainingBudget - resBudget,
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


float getMaxPossiblePayoffOptimized(float remainingBudget, float payoffDownwards = 0.0);


int main()
{
	// ---------------- Ввод --------------------
	std::ifstream inputFile("input.txt");
	if (!inputFile) {
		std::cerr << "Не удалось открыть файл!" << std::endl;
		return 1;
	}

	inputFile >> specAmount;

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


	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::cout << getMaxPossiblePayoff(budget) << std::endl;
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Standart Diff(ms) = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0 << std::endl;


	/*std::cout << "Brigades: ";
	for (Brigade brigade : bestBrigades) {
		std::cout << brigade.toString() << " ";
	}
	std::cout << std::endl;
	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		if (i == 0) std::cout << "Engieneers   - ";
		if (i == 1) std::cout << "Miners       - ";
		if (i == 2) std::cout << "Transporters - ";

		for (int j : bestReservation[i]) {
			std::cout << j << " ";
		}

		std::cout << std::endl;
	}
	std::cout << "Add-on workers - ";
	getLeftoverSpecsAmountForBudget(gRemainingBudget, bestReservation, true);
	std::cout << std::endl;*/


	begin = std::chrono::steady_clock::now();
	std::cout << getMaxPossiblePayoffOptimized(budget) << std::endl;
	end = std::chrono::steady_clock::now();

	std::cout << "Optimized Diff(ms) = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0 << std::endl;


	delete[] specDayPay;
	delete[] types;

	return 0;
}
