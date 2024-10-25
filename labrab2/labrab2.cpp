/******************************************************************************

							  Online C++ Compiler.
			   Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>

const int SPEC_TYPES_AMOUNT = 3;
enum specTypes { Engieneer, Miner, Transporter };

const int specAmount = 30;
float* specDayPay;
specTypes* types;

const float budget = 3000; //3500;

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
			cost += specialist.first;
			reservation[i].push_back(specialist.second);
			counts[i]++;

			if (--count == 0) {
				break;
			}
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


int getLeftoverSpecsAmountForBudget(float budget) {
	float remainingBudget = budget;
	int amount = 0;

	std::vector<std::pair<float, int>> specialists;

	for (int i = 0; i < SPEC_TYPES_AMOUNT; i++) {
		for (int j = 0; j < specAmount; j++) {
			if (std::find(reservation[i].begin(), reservation[i].end(), j) == reservation[i].end()) {
				specialists.push_back({ specDayPay[j], j });
			}
		}
	}

	std::sort(specialists.begin(), specialists.end()); // сортировка по 1-у элементу пары

	for (const auto& specialist : specialists) {
		budget -= specialist.first;
		if (budget >= 0) {
			amount++;
		}
		else {
			break;
		}
	}

	return amount;
}


float getMaxPossiblePayoff(float remainingBudget) {
	float payoff = 0.0;

	payoff = getLeftoverSpecsAmountForBudget(remainingBudget) * specBonus;

	for (int i = 0; i < BRIGADES_AMOUNT; i++) {
		float localPayoff = 0.0;
		float resBudget = reserveSpecs(brigades[i]);

		if (resBudget > 0 && remainingBudget - resBudget >= 0) {
			localPayoff = brigades[i].getBrigadePayoff()
				+ getMaxPossiblePayoff(remainingBudget - resBudget);

			if (localPayoff > payoff) {
				payoff = localPayoff;
			}
		}

		if (resBudget > 0) unreserveSpecs(brigades[i]);
	}

	return payoff;
}


int main()
{
	//specAmount = 30; // >= 30
	float specDayPay1[specAmount] =
	{
		100, 120, 140, 130, 110, 90,  105.6, 87.8,  142.1, 121.4,
		102, 110, 130, 124, 119, 98,  102.6, 97.8,  122.1, 111.4,
		123, 133, 144, 127, 101, 126, 145.6, 147.8, 143.2, 131.4
	};
	specTypes types1[specAmount] =
	{
		Engieneer,      Engieneer,      Engieneer,      Engieneer,      Engieneer,
		Engieneer,      Engieneer,      Engieneer,      Engieneer,      Engieneer,
		Miner,          Miner,          Miner,          Miner,          Miner,
		Miner,          Miner,          Miner,          Miner,          Miner,
		Transporter,    Transporter,    Transporter,    Transporter,    Transporter,
		Transporter,    Transporter,    Transporter,    Transporter,    Transporter
	};

	specDayPay = specDayPay1;
	types = types1;

	std::cout << getMaxPossiblePayoff(budget) << std::endl;

	return 0;
}
