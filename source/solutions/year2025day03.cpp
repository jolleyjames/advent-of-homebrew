#include <string>
#include <vector>
#include <cstdlib>

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

    std::int64_t maxJoltage(const std::string& bank, const std::size_t size) {
        std::vector<std::size_t> xvec(size);
        for (std::size_t i = 0; i < xvec.size(); i++)
            xvec[i] = i;
        for (std::size_t i = 1; i < bank.size()-size+1; i++) {
            for (std::size_t j = 0; j < size; j++) {
                if (bank[i+j] > bank[xvec[j]]) {
                    for (std::size_t jj = j; jj < size; jj++)
                        xvec[jj] = i+jj;
                    break;
                }
            }
        }
        std::int64_t joltage = 0;
        for (std::size_t i = 0; i < xvec.size(); i++) {
            joltage *= 10;
            joltage += static_cast<std::int64_t>(bank[xvec[i]]-'0');
        }
        return joltage;
    }

    std::int64_t generalized(std::ifstream& in, const std::size_t size) {
        std::vector<std::string> banks = loadBanks(in);
        std::int64_t sum = 0;
        for (auto bank : banks) {
            std::int64_t joltage = maxJoltage(bank, size);
            sum += joltage;
        }
        return sum;
    }

    advhb::Solution s1(
        2025,
        3,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in,2))}; }
    );

    advhb::Solution s2(
        2025,
        3,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in,12))}; }
    );
}
