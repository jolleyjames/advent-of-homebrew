#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <tuple>
#include <algorithm>
#include <map>
#include <set>

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

    typedef std::tuple<std::int64_t, std::size_t, std::size_t> Distance;
    std::vector<Distance> findDistancesSq(const Eigen::Array<std::int64_t, Eigen::Dynamic, 3>& junctionBoxes) {
        std::vector<Distance> distances;
        // return tuple of {squared_distance, jb_index_1, jb_index_2}
        for (Eigen::Index from = 0; from < junctionBoxes.rows()-1; from++) {
            Eigen::Array<std::int64_t, 1, 3> fromRow = junctionBoxes.row(from);
            Eigen::Array<std::int64_t, Eigen::Dynamic, 3> toRows = 
                junctionBoxes.block<Eigen::Dynamic, 3>(from+1, 0, junctionBoxes.rows()-from-1, 3);
            Eigen::Array<std::int64_t, Eigen::Dynamic, 3> diff = toRows.rowwise() - fromRow;
            Eigen::Array<std::int64_t, Eigen::Dynamic, 3> diffSq = diff * diff;
            Eigen::Vector<std::int64_t, Eigen::Dynamic> sums = diffSq.rowwise().sum();
            for (Eigen::Index to = 0; to < sums.size(); to++)
                distances.push_back({sums[to], (std::size_t)from, (std::size_t)(from+1+to)});
        }
        return distances;
    }

    std::size_t part1(std::ifstream& in) {
        auto jb = loadJunctionBoxes(in);
        std::vector<Distance> distances = findDistancesSq(jb);
        std::sort(distances.begin(), distances.end(), 
          [](const Distance& a, const Distance& b){
            return std::get<0>(a) < std::get<0>(b);
          });
        // For test mode, combine 10 closest pairs; for normal mode, 1000
        std::size_t combinations = 10;
        if (jb.rows() > 20)
            combinations = 1000;
        // Map each junction box index to its circuit
        std::map<std::size_t, std::set<std::size_t>*> boxCircuitMap;
        for (Eigen::Index x = 0; x < jb.rows(); x++) {
            std::set<std::size_t>* circuit = new std::set<std::size_t>();
            circuit->insert((std::size_t)x);
            boxCircuitMap[x] = circuit;
        }
        // find the C shortest distances and combine the pairs into a single circuit
        for (std::size_t c = 0; c < combinations; c++) {
            const Distance& d = distances[c];
            // are they not neighbors?
            std::set<std::size_t>* c1 = boxCircuitMap[std::get<1>(d)];
            std::set<std::size_t>* c2 = boxCircuitMap[std::get<2>(d)];
            if (c1 != c2) {
                // they're neighbors now!
                // merge the circuits
                c1->insert(c2->begin(), c2->end());
                // update the circuit mapping
                for (auto jbndx : *c2)
                    boxCircuitMap[jbndx] = c1;
                // delete the now-unused circuit
                delete c2;
            }
        }
        // get the unique set of circuits, saving them in a set sorted by descending circuit length
        auto circuitComparator = [](std::set<std::size_t>* a, std::set<std::size_t>* b) {
            return a->size() > b->size();
        };
        std::set<std::set<std::size_t>*, decltype(circuitComparator)> uniqueCircuits(circuitComparator);        
        for (auto entry : boxCircuitMap)
            uniqueCircuits.insert(entry.second);
        // find product of 3 largest circuits
        std::size_t product = 1;
        auto iter = uniqueCircuits.begin();
        for (int n = 0; n < 3; n++) {
            product *= (*iter)->size();
            iter++;
        }
        for (auto circuit : uniqueCircuits)
            delete circuit;
        return product;
    }

    advhb::Solution s1(
        2025,
        8,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );

}
