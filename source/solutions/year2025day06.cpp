#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <string>
#include <tuple>

#include <iostream>

#include <stdexcept>
#include <algorithm>
#include <list>
#include <iterator>

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

    std::int64_t part1(std::ifstream &in) {
        Homework hw = loadHomework(in);
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
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );

}
