#include <string>
#include <fstream>
#include <vector>
#include <numeric>
#include <map>

#include "solution.hpp"

namespace y2023d01 {

    std::vector<std::string> fileToStrings(std::ifstream& in) {
        std::string line;
        std::vector<std::string> v;
        while (std::getline(in, line))
            v.push_back(line);
        return v;
    }

    const std::map<std::string, std::string> convert({
        {"one", "1"},
        {"two", "2"},
        {"three", "3"},
        {"four", "4"},
        {"five", "5"},
        {"six", "6"},
        {"seven", "7"},
        {"eight", "8"},
        {"nine", "9"}
    });

    int findFirstLast(const std::string& s, const std::vector<std::string>& values) {
        size_t firstNdx = std::string::npos, lastNdx = std::string::npos;
        std::string first, last;
        for (auto value : values) {
            size_t found = s.find(value);
            if (found != std::string::npos) {
                if (firstNdx == std::string::npos || found < firstNdx) {
                    firstNdx = found;
                    first = value;
                }
                found = s.rfind(value);
                if (lastNdx == std::string::npos || (found + value.size()) > lastNdx) {
                    lastNdx = found + value.size();
                    last = value;
                }
            }
        }
        if (convert.find(first) != convert.end())
            first = convert.at(first);
        if (convert.find(last) != convert.end())
            last = convert.at(last);
        return std::stoi(first + last);
    }

    const std::vector<std::string> numValues({
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
    });
    const std::vector<std::string> alphaNumValues({
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
    });

    int generalized(std::ifstream& in, const std::vector<std::string>& values) {
        std::vector<std::string> v = fileToStrings(in);
        int result = 0;
        for (auto s : v)
            result += findFirstLast(s, values);
        return result;
    }

    int part1(std::ifstream& in) {
        return generalized(in, numValues);
    }

    advhb::Solution s1(2023, 1, advhb::PuzzlePart::PartOne, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(part1(in))};}
    );

    int part2(std::ifstream& in) {
        return generalized(in, alphaNumValues);
    }

    advhb::Solution s2(2023, 1, advhb::PuzzlePart::PartTwo, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(part2(in))};}
    );

}
