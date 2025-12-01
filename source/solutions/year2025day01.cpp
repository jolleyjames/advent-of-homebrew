#include <vector>
#include <string>
#include <fstream>

#include "solution.hpp"

namespace y2025d01 {

    std::vector<int> loadRotations(std::ifstream& in) {
        std::vector<int> rotations;
        std::string rotation;
        while(std::getline(in, rotation)) {
            int n = std::stoi(rotation.substr(1));
            if (rotation[0] == 'L')
                n *= -1;
            rotations.push_back(n);
        }
        return rotations;
    }

    const int VALUES = 100;
    const int START = 50;
    const int MATCH = 0;
    int part1(std::ifstream& in) {
        std::vector<int> rotations = loadRotations(in);
        int value = START;
        int matches = 0;
        for (int rotation : rotations) {
            value = ((value + rotation) % VALUES);
            if (value == MATCH)
                matches++;
        }
        return matches;
    }

    advhb::Solution s1(
        2025,
        1,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part1(in))};}
    );

    int part2(std::ifstream& in) {
        std::vector<int> rotations = loadRotations(in);
        int value = START;
        int matches = 0;
        for (int rotation : rotations) {
            if (rotation < 0 && value == 0)
                value = VALUES;
            int passes = 0;
            value += rotation;
            if (rotation >= 0)
                passes = value / VALUES;
            else if (value <= 0)
                passes = (-value / VALUES) + 1;            
            matches += passes;
            value %= VALUES;
            if (value < 0)
                value += VALUES;
        }
        return matches;
    }

    advhb::Solution s2(
        2025,
        1,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part2(in))};}
    );




}
