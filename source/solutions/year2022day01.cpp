#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "solution.hpp"

namespace y2022d01 {

    int processFile(std::ifstream& in, size_t count) {
        std::vector<int> v;
        int total = 0;
        std::string line;
        while (std::getline(in, line)) {
            if (line == "") {
                v.push_back(total);
                total = 0;
            } else {
                total += std::stoi(line);
            }
        }
        if (total > 0)
            v.push_back(total);
        in.close();
        // sort in descending order
        std::sort(v.begin(), v.end(), std::greater<int>());
        // return sum of top "count" values
        return std::accumulate(v.begin(), v.begin()+count, 0);
    }

    advhb::Solution s1(
        2022, 
        1, 
        advhb::PuzzlePart::PartOne, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(processFile(in,1))};}
    );

    advhb::Solution s2(
        2022, 
        1, 
        advhb::PuzzlePart::PartTwo, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(processFile(in,3))};}
    );
}
