#include <string>
#include <set>

#include "solution.hpp"

namespace y2025d04
{
    std::set<std::tuple<std::size_t, std::size_t>> loadRolls(std::ifstream &in) {
        std::set<std::tuple<std::size_t, std::size_t>> rolls;
        std::string row;
        std::size_t y = 0;
        while(std::getline(in, row)) {
            for (std::size_t x = 0; x < row.size(); x++) {
                if (row[x] == '@')
                    rolls.insert({y, x});
            }
            y++;
        }
        return rolls;
    }

    std::size_t start(std::size_t n) {
        if (n == 0)
            return 0;
        return n-1;
    }

    bool isRollAccessible(
        const std::tuple<std::size_t, std::size_t>& roll,
        const std::set<std::tuple<std::size_t, std::size_t>>& allRolls
    ) {
        int adjacents = 0;
        for (std::size_t y = start(std::get<0>(roll)); y <= std::get<0>(roll)+1; y++) {
            for (std::size_t x = start(std::get<1>(roll)); x <= std::get<1>(roll)+1; x++) {
                if (y != std::get<0>(roll) || x != std::get<1>(roll)) {
                    if (allRolls.find({y,x}) != allRolls.end()) {
                        adjacents++;
                    }
                    if (adjacents >= 4) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    std::set<std::tuple<std::size_t, std::size_t>> findAccessibleRolls(std::set<std::tuple<std::size_t, std::size_t>>& rolls) {
        std::set<std::tuple<std::size_t, std::size_t>> accessibleRolls;
        for (const auto& roll : rolls) {
            if (isRollAccessible(roll, rolls))
                accessibleRolls.insert(roll);
        }
        return accessibleRolls;

    }

    std::size_t generalized(std::ifstream &in, advhb::PuzzlePart part) {
        std::set<std::tuple<std::size_t, std::size_t>> rolls = loadRolls(in);
        std::size_t totalAccessibleRolls = 0;
        std::set<std::tuple<std::size_t, std::size_t>> accessibleRolls;
        do {
            accessibleRolls = findAccessibleRolls(rolls);
            totalAccessibleRolls += accessibleRolls.size();
            if (part == advhb::PuzzlePart::PartTwo)
                for (auto accRoll : accessibleRolls)
                    rolls.erase(accRoll);
        } while (part == advhb::PuzzlePart::PartTwo && !accessibleRolls.empty());
        return totalAccessibleRolls;
    }

    advhb::Solution s1(
        2025,
        4,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartOne))}; }
    );

    advhb::Solution s2(
        2025,
        4,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartTwo))}; }
    );

}
