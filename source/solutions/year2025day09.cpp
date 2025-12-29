#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <iostream>

#include "solution.hpp"

namespace y2025d09
{
    struct Coord {
        std::int64_t x;
        std::int64_t y;
    };

    Coord operator-(const Coord& minuend, const Coord& subtrahend) {
        Coord diff;
        diff.x = minuend.x - subtrahend.x;
        diff.y = minuend.y - subtrahend.y;
        return diff;
    }

    std::ostream& operator<<(std::ostream& outstream, const Coord& coord) {
        outstream << "(" << coord.x << ", " << coord.y << ")";
        return outstream;
    }

    bool operator==(const Coord& a, const Coord& b) {
        return a.x == b.x && a.y == b.y;
    }

    std::int64_t top(const Coord& a, const Coord& b) {
        return std::min(a.y, b.y);
    }

    std::int64_t bottom(const Coord& a, const Coord& b) {
        return std::max(a.y, b.y);
    }

    std::int64_t left(const Coord& a, const Coord& b) {
        return std::min(a.x, b.x);
    }

    std::int64_t right(const Coord& a, const Coord& b) {
        return std::max(a.x, b.x);
    }

    struct CoordHash {
        std::size_t operator()(const Coord& coord) const {
            return (std::size_t)(coord.x * coord.y);        
        }
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

    std::ostream& operator<<(std::ostream& outstream, const std::tuple<Coord,Coord>& combo) {
        outstream << "{" << std::get<0>(combo) << ", " << std::get<1>(combo) << "}";
        return outstream;
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

    typedef std::map<std::int64_t, std::vector<std::int64_t>> EdgeMap;
    std::tuple<EdgeMap,EdgeMap> findEdges(const std::vector<Coord>& coords) {
        EdgeMap xEdges;
        EdgeMap yEdges;
        for (std::size_t n = 0; n < coords.size(); n++) {
            const auto& a = coords[n];
            const auto& b = coords[(n+1)%coords.size()];
            auto diff = b - a;
            std::int64_t key, from, to;
            EdgeMap* edgeMapPtr = nullptr;
            if (diff.x == 0) {
                key = a.x;
                from = std::min(a.y, b.y);
                to = std::max(a.y, b.y);
                edgeMapPtr = &xEdges;
            } else {
                key = a.y;
                from = std::min(a.x, b.x);
                to = std::max(a.x, b.x);
                edgeMapPtr = &yEdges;
            }
            (*edgeMapPtr)[key].push_back(from);
            (*edgeMapPtr)[key].push_back(to);
        }
        return {xEdges, yEdges};
    }

    std::unordered_set<Coord, CoordHash> inLoop;
    std::unordered_set<Coord, CoordHash> outLoop;
    bool isWithinLoop(
        const Coord& vertex, 
        const EdgeMap& xEdges, 
        const EdgeMap& yEdges,
        bool towardPositiveX, 
        bool towardPositiveY
    ) {
        if (outLoop.count(vertex) > 0) {
            //DEBUG
            std::cout << "outLoop contains " << vertex << std::endl;
            return false;
        }
        else if (inLoop.count(vertex) > 0) {
            //DEBUG
            std::cout << "inLoop contains " << vertex << std::endl;
            return true;
        }
        std::size_t xFinds = 0;
        const std::int64_t CONST_Y = vertex.y;
        EdgeMap::const_iterator xEdgeItr = xEdges.find(vertex.x);
        for (;;) {
            for (std::size_t n = 0; n < xEdgeItr->second.size(); n+=2) {
                if (xEdgeItr->second[n] <= CONST_Y && CONST_Y <= xEdgeItr->second[n+1]) {
                    xFinds++;
                    break;
                }
            }
            if (towardPositiveX) {
                xEdgeItr++;
                if (xEdgeItr == xEdges.cend())
                    break;
            } else {
                if (xEdgeItr == xEdges.begin())
                    break;
                xEdgeItr--;
            }
        }
        // if an even number of edges found, vertex is outside the loop
        if (xFinds%2 == 0) {
            //DEBUG
            std::cout << vertex << " xFinds=" << xFinds << " adding to outLoop" << std::endl;
            outLoop.insert(vertex);
            return false;
        }
        // now check in Y direction
        std::size_t yFinds = 0;
        const std::int64_t CONST_X = vertex.x;
        EdgeMap::const_iterator yEdgeItr = yEdges.find(vertex.y);
        for (;;) {
            for (std::size_t n = 0; n < yEdgeItr->second.size(); n+=2) {
                if (yEdgeItr->second[n] <= CONST_X && CONST_X <= yEdgeItr->second[n+1]) {
                    yFinds++;
                    break;
                }
            }
            if (towardPositiveY) {
                yEdgeItr++;
                if (yEdgeItr == yEdges.cend())
                    break;
            } else {
                if (yEdgeItr == yEdges.begin())
                    break;
                yEdgeItr--;
            }
        }
        // if an even number of edges found, vertex is outside the loop
        if (yFinds%2 == 0) {
            //DEBUG
            std::cout << vertex << " yFinds=" << yFinds << " adding to outLoop" << std::endl;
            outLoop.insert(vertex);
            return false;
        }
        // odd number of edges in both x and y directions.
        // the vertex is in the loop.
        //DEBUG
        std::cout << vertex << " xFinds=" << xFinds << " yFinds=" << yFinds << " adding to inLoop" << std::endl;
        inLoop.insert(vertex);
        return true;
    }

    bool isWithinLoop(const std::tuple<Coord,Coord>& combo, const EdgeMap& xEdges, const EdgeMap& yEdges) {
        Coord a = std::get<0>(combo);
        Coord b = std::get<1>(combo);
        Coord topLeft = {left(a,b), top(a,b)};
        Coord checkCoords[2];
        bool isFirstCheckTopLeft = false;
        if (topLeft == a || topLeft == b) {
            checkCoords[0] = {left(a,b), bottom(a,b)};
            checkCoords[1] = {right(a,b), top(a,b)};
        } else {
            isFirstCheckTopLeft = true;
            checkCoords[0] = topLeft;
            checkCoords[1] = {right(a,b), bottom(a,b)};
        }
        // first check
        bool is = isWithinLoop(
            checkCoords[0],
            xEdges,
            yEdges,
            false,
            !isFirstCheckTopLeft
        );
        if (!is)
            return false;
        // second check
        is = isWithinLoop(
            checkCoords[1],
            xEdges,
            yEdges,
            true,
            isFirstCheckTopLeft
        );
        return is;
    }

    std::int64_t part2(std::ifstream& in) {
        auto coords = loadRedTiles(in);
        auto edgeMaps = findEdges(coords);
        auto xEdges = std::get<0>(edgeMaps);
        auto yEdges = std::get<1>(edgeMaps);
        auto combos = combineCoords(coords);
        sortCombosByArea(combos);
        for (const auto& combo : combos) {
            //DEBUG
            std::cout << combo << std::endl;
            if (isWithinLoop(combo, xEdges, yEdges)) {
                // clear out cache before another run of part2
                inLoop.clear();
                outLoop.clear();
                return getArea(combo);
            }
        }
        // shouldn't be reachable with valid input
        inLoop.clear();
        outLoop.clear();
        return 0;
    }

    advhb::Solution s2(
        2025,
        9,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part2(in))}; }
    );




}
