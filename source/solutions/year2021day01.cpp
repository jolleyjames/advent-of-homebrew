#include <vector>
#include <fstream>

#include <Eigen/Dense>

#include "solution.hpp"

namespace y2021d01 {

    Eigen::VectorXi fileToVectorXi(std::ifstream& in) {
        std::vector<int> v;
        int x;
        while (in >> x)
            v.push_back(x);
        Eigen::Map<Eigen::VectorXi> m(v.data(), v.size());
        return m;
    }

    int generalized(std::ifstream& in, int windowsize) {
        Eigen::VectorXi v = fileToVectorXi(in);
        if (in.is_open()) in.close();
        Eigen::VectorXi v1 = v.head(v.size()-windowsize);
        Eigen::VectorXi v2 = v.tail(v.size()-windowsize);
        Eigen::VectorXi diff = v2 - v1;
        return (diff.array() > 0).count();
    }

    std::vector<std::string> generalizedToStrVec(std::ifstream& in, int windowsize) {
        return {std::to_string(generalized(in, windowsize))};
    }

    advhb::Solution s1(
        2021, 
        1, 
        advhb::PuzzlePart::PartOne,
        [](std::ifstream& in) {return generalizedToStrVec(in, 1);}
    );
    
    advhb::Solution s2(
        2021, 
        1, 
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream& in) {return generalizedToStrVec(in, 3);}
    );
}
