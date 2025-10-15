#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <Eigen/Dense>

#include "solution.hpp"

namespace y2021d03 {

    Eigen::MatrixXi fileToMatrix(std::ifstream& in) {
        size_t columnCount = 0;
        std::vector<Eigen::RowVectorXi> stdv;
        std::string s;
        while (in >> s) {
            if (columnCount == 0)
                columnCount = s.length();
            Eigen::RowVectorXi row(columnCount);
            for (size_t i = 0; i < columnCount; i++) {
                row(i) = (int)(s[i] - '0');
            }
            stdv.push_back(row);
        }
        Eigen::MatrixXi m(stdv.size(), columnCount);
        for (size_t i = 0; i < stdv.size(); i++)
            m.row(i) = stdv[i];
        return m;
    }

    int bitRowToInt(const Eigen::RowVectorXi& row) {
        int value = 0;
        for (auto i = 0; i < row.size(); i++) {
            value <<= 1;
            value += row(i);
        }
        return value;
    }

    Eigen::RowVectorXi findCommonBits (const Eigen::MatrixXi& m) {
        Eigen::RowVectorXi colsums = m.colwise().sum();
        return colsums * 2 / m.rows();
    }

    Eigen::RowVectorXi bitFlip(const Eigen::RowVectorXi& row) {
        // assumes RowVector values are all 0 or 1.
        return row ^ Eigen::RowVectorXi::Ones(row.size());
    }

    int part1(std::ifstream& in) {
        Eigen::MatrixXi m = fileToMatrix(in);
        in.close();
        Eigen::RowVectorXi commonBits = findCommonBits(m);
        Eigen::RowVectorXi uncommonBits = bitFlip(commonBits);
        int gamma = bitRowToInt(commonBits),
            epsilon = bitRowToInt(uncommonBits);
        return gamma * epsilon;
    }

    Eigen::MatrixXi reduce(const Eigen::MatrixXi& m, Eigen::Index colIndex, bool least=false) {
        Eigen::RowVectorXi commonBits = findCommonBits(m);
        if (least)
            commonBits = bitFlip(commonBits);
        // are we keeping 1s or 0s?
        int rowKeepCount = m.col(colIndex).sum();
        if (commonBits(colIndex) == 0) {
            rowKeepCount = m.rows() - rowKeepCount;
        }
        Eigen::MatrixXi newmat(rowKeepCount, m.cols());
        Eigen::Index nextrow = 0;
        for (auto i = 0; i < m.rows() && nextrow < rowKeepCount; i++) {
            if (m(i, colIndex) == commonBits(colIndex)) {
                newmat.row(nextrow) = m.row(i);
                nextrow++;
            }
        }
        return newmat;
    }

    Eigen::RowVectorXi findRating(Eigen::MatrixXi m, bool least=false) {
        Eigen::Index colIndex = 0;
        while (colIndex < m.cols() && m.rows() > 1) {
            m = reduce(m, colIndex, least);
            colIndex++;
        }
        if (m.rows() == 1)
            return m.row(0);
        throw std::runtime_error("input error: could not reduce to a single row");
    }

    int part2(std::ifstream& in) {
        Eigen::MatrixXi m = fileToMatrix(in);
        in.close();
        int oxygen = bitRowToInt(findRating(m));
        int co2 = bitRowToInt(findRating(m, true));
        return oxygen * co2;
     }

    advhb::Solution s1(
        2021, 
        3, 
        advhb::PuzzlePart::PartOne, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(part1(in))};}
    );

    advhb::Solution s2(
        2021, 
        3, 
        advhb::PuzzlePart::PartTwo, 
        [](std::ifstream& in) {return std::vector<std::string> {std::to_string(part2(in))};}
    );
}
