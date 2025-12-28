#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cstdlib>

#include "solution.hpp"

namespace y2025d09
{
    struct Coord {
        std::int64_t x;
        std::int64_t y;
    };

    std::vector<Coord> loadRedTiles(std::ifstream& in) {
        std::string line;
        std::vector<Coord> coords;
        while (std::getline(in, line)) {
            Coord coord;
            std::size_t comma = line.find(',');
            coord.x = std::stoll(line.substr(0,comma));
            coord.y = std::stoll(line.substr(comma+1));
            coords.push_back(coord);
        }
        return coords;
    }

    std::vector<std::tuple<Coord,Coord>> combineCoords(const std::vector<Coord>& coords) {
        std::vector<std::tuple<Coord,Coord>> combos;
        for (std::size_t i = 0; i < coords.size()-1; i++) {
            for (std::size_t j = i+1; j < coords.size()-1; j++) {
                combos.push_back({coords[i], coords[j]});
            }
        }
        return combos;
    }

    std::int64_t getArea(const Coord& a, const Coord& b) {
        return (std::abs(a.x-b.x)+1)*(std::abs(a.y-b.y)+1);
    }

    std::int64_t getArea(const std::tuple<Coord,Coord>& coords) {
        return getArea(std::get<0>(coords), std::get<1>(coords));
    }

    void sortCombosByArea(std::vector<std::tuple<Coord,Coord>>& combos) {
        std::sort(combos.begin(), combos.end(),
          [](const auto& coordPair1, const auto& coordPair2) {
            return getArea(coordPair1) > getArea(coordPair2);
          });
    }

    std::int64_t part1(std::ifstream& in) {
        auto combos = combineCoords(loadRedTiles(in));
        sortCombosByArea(combos);
        return getArea(combos[0]);
    }

    advhb::Solution s1(
        2025,
        9,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );

}
