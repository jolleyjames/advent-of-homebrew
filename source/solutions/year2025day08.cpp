#include <fstream>
#include <string>
#include <cstdint>
#include <vector>

#include <Eigen/Dense>

//DEBUG
#include <iostream>

#include "solution.hpp"

namespace y2025d08
{

    Eigen::Array<std::int64_t, Eigen::Dynamic, 3> loadJunctionBoxes(std::ifstream& in) {
        std::string line;
        std::vector<std::int64_t> coeffs;
        while (std::getline(in, line)) {
            std::size_t comma1 = line.find_first_of(',');
            std::size_t comma2 = line.find_last_of(',');
            coeffs.push_back(std::stoll(line.substr(0,comma1)));
            coeffs.push_back(std::stoll(line.substr(comma1+1,comma2-comma1)));
            coeffs.push_back(std::stoll(line.substr(comma2+1)));
        }
        Eigen::Array<std::int64_t, Eigen::Dynamic, 3> jb(coeffs.size()/3, 3);
        for (Eigen::Index i = 0; i < jb.rows(); i++) {
            Eigen::RowVector<std::int64_t, 3> row;
            row << coeffs[3*i], coeffs[3*i+1], coeffs[3*i+2];
            jb.row(i) = row;
        }
        return jb;
    }

    std::int64_t testLoad(std::ifstream& in) {
        auto jb = loadJunctionBoxes(in);
        std::cout << jb << std::endl;
        return 1;
    }

    advhb::Solution s1(
        2025,
        8,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(testLoad(in))}; }
    );

}
