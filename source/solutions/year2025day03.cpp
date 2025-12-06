#include <string>
#include <vector>
#include <cstdlib>
//DEBUG
#include <iostream>

#include "solution.hpp"

namespace y2025d03
{
    std::vector<std::string> loadBanks(std::ifstream &in) {
        std::vector<std::string> banks;
        std::string bank;
        while(std::getline(in, bank))
            banks.push_back(bank);
        return banks;
    }

    int maxJoltage(const std::string& bank) {
        std::size_t x0 = 0;
        std::size_t x1 = 1;
        for (std::size_t i = 1; i < bank.size()-1; i++) {
            if (bank[x0] != '9' && bank[i] > bank[x0]) {
                x0 = i;
                x1 = i+1;
            }
            else if (bank[i+1] > bank[x1])
                x1 = i+1;
            if (bank[x0] == '9' and bank[x1] == '9')
                break;
        }
        return 10*static_cast<int>(bank[x0]-'0') + static_cast<int>(bank[x1]-'0');
    }

    int part1(std::ifstream &in) {
        std::vector<std::string> banks = loadBanks(in);
        int sum = 0;
        for (auto bank : banks) {
            int joltage = maxJoltage(bank);
            //DEBUG
            std::cout << joltage << std::endl;
            sum += joltage;
        }
        return sum;
    }

    advhb::Solution s1(
        2025,
        3,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );

}
