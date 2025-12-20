#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
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

    std::int64_t getArea(const Coord& a, const Coord& b) {
        return (std::abs(a.x-b.x)+1)*(std::abs(a.y-b.y)+1);
    }

    std::int64_t part1(std::ifstream& in) {
        auto coords = loadRedTiles(in);
        std::int64_t maxArea = 0;
        for (std::size_t a = 0; a < coords.size()-1; a++) {
            for (std::size_t b = a+1; b < coords.size(); b++) {
                auto area = getArea(coords[a], coords[b]);
                if (area > maxArea)
                    maxArea = area;
            }
        }
        return maxArea;
    }

    advhb::Solution s1(
        2025,
        9,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );

}
