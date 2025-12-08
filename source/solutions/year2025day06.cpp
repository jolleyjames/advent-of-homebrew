#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <string>
#include <tuple>

#include "solution.hpp"

namespace y2025d06
{
    typedef std::tuple< std::vector<std::vector<std::int64_t>>, std::vector<std::string> > Homework;

    Homework loadHomework(std::ifstream &in) {
        std::string line;
        std::vector<std::vector<std::int64_t>> columns;
        std::vector<std::string> ops;
        while (std::getline(in, line)) {
            bool firstLine = false;
            bool loadOps = false;
            if (columns.empty())
                firstLine = true;
            else if (line[0] == '+' || line[0] == '*')
                loadOps = true;
            std::stringstream ss(line);
            std::size_t colIndex = 0;
            while(!ss.eof()) {
                std::string s;
                ss >> s;
                if (s.size() == 0)
                    continue;
                if (loadOps)
                    ops.push_back(s);
                else {
                    if (firstLine)
                        columns.push_back(std::vector<std::int64_t>());
                    std::int64_t i = std::stoll(s);
                    columns[colIndex].push_back(i);
                }
                colIndex++;
            }                
        }
        return {columns, ops};
    }

    Homework loadHomework2(std::ifstream &in) {
        std::vector<std::string> lines;
        std::string s;
        std::vector<std::vector<std::int64_t>> columns;
        std::vector<std::string> ops;
        while (std::getline(in, s))
            lines.push_back(s);
        std::vector<std::int64_t> columnGroup;
        for (std::size_t n = lines[0].size(); n > 0; n--) {
            s = "";
            for (std::size_t j = 0; j < lines.size(); j++) {
                s += lines[j][n-1];
            }
            if (s.find_first_not_of(' ') != std::string::npos)
                columnGroup.push_back(std::stoll(s));
            if (s[s.size()-1] == '+' || s[s.size()-1] == '*') {
                ops.push_back(s.substr(s.size()-1));
                columns.push_back(columnGroup);
                columnGroup.clear();
            }
        }
        return {columns, ops};
    }

    std::int64_t generalized(std::ifstream &in, advhb::PuzzlePart part) {
        Homework hw;
        if (part == advhb::PuzzlePart::PartOne)
            hw = loadHomework(in);
        else
            hw = loadHomework2(in);
        auto columns = std::get<0>(hw);
        auto ops = std::get<1>(hw);
        std::int64_t total = 0;
        for (std::size_t x = 0; x < ops.size(); x++) {
            std::int64_t result = 0;
            if (ops[x] == "*")
                result = 1;
            for (auto n : columns[x]) {
                if (ops[x] == "+")
                    result += n;
                else
                    result *= n;
            }
            total += result;
        }
        return total;
    }

    advhb::Solution s1(
        2025,
        6,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartOne))}; }
    );

    advhb::Solution s2(
        2025,
        6,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartTwo))}; }
    );
}
