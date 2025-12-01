#include <vector>
#include <string>
#include <fstream>

#include "solution.hpp"

namespace y2025d01
{

    std::vector<int> loadRotations(std::ifstream &in)
    {
        std::vector<int> rotations;
        std::string rotation;
        while (std::getline(in, rotation))
        {
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

    int applyRotation(int rotation, int &value)
    {
        value = ((value + rotation) % VALUES);
        if (value == MATCH)
            return 1;
        else
            return 0;
    }

    int applyRotation434C49434B(int rotation, int &value)
    {
        if (rotation < 0 && value == 0)
            value = VALUES;
        int passes = 0;
        value += rotation;
        if (rotation >= 0)
            passes = value / VALUES;
        else if (value <= 0)
            passes = (-value / VALUES) + 1;
        value %= VALUES;
        if (value < 0)
            value += VALUES;
        return passes;
    }

    int generalized(std::ifstream &in, advhb::PuzzlePart part)
    {
        std::vector<int> rotations = loadRotations(in);
        int value = START;
        int matches = 0;
        for (int rotation : rotations)
        {
            if (part == advhb::PuzzlePart::PartOne)
                matches += applyRotation(rotation, value);
            else if (part == advhb::PuzzlePart::PartTwo)
                matches += applyRotation434C49434B(rotation, value);
        }
        return matches;
    }

    advhb::Solution s1(
        2025,
        1,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in,advhb::PuzzlePart::PartOne))}; }
    );

    advhb::Solution s2(
        2025,
        1,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in,advhb::PuzzlePart::PartTwo))}; }
    );
}
