#include <fstream>
#include <string>
#include <set>
#include <tuple>
#include <cstdint>
#include <map>

#include "solution.hpp"

namespace y2025d07
{

    typedef std::tuple<std::size_t, std::size_t> Loc;

    std::tuple<Loc, std::set<Loc>, std::size_t> loadDiagram(std::ifstream &in) {
        std::string line;
        Loc start;
        std::set<Loc> splitters;
        std::size_t y = 0;
        while (std::getline(in, line)) {
            for (std::size_t x = 0; x < line.size(); x++) {
                if (line[x] == '^')
                    splitters.insert({y,x});
                else if (line[x] == 'S')
                    start = {y,x};
            }
            y++;
        }
        return {start, splitters, y};
    }

    std::uint64_t generalized(std::ifstream &in, advhb::PuzzlePart part) {
        auto diagram = loadDiagram(in);
        Loc start = std::get<0>(diagram);
        std::set<Loc> splitters = std::get<1>(diagram);
        std::size_t yEndAt = std::get<2>(diagram);
        std::map<Loc, std::uint64_t> beamsAndPaths = {{start,1}};
        std::uint64_t splits = 0;
        for (std::size_t y = 0; y < yEndAt; y++) {
            std::map<Loc, std::uint64_t> newBeamsAndPaths;
            for (const auto& beamAndPaths : beamsAndPaths) {
                auto beam = std::get<0>(beamAndPaths);
                auto paths = std::get<1>(beamAndPaths);
                if (splitters.find(beam) != splitters.end()) {
                    splits++;
                    Loc nextBeamL = {std::get<0>(beam)+1, std::get<1>(beam)-1};
                    Loc nextBeamR = {std::get<0>(beam)+1, std::get<1>(beam)+1};
                    auto i = newBeamsAndPaths.find(nextBeamL);
                    if (i != newBeamsAndPaths.end())
                        i->second += paths;
                    else
                        newBeamsAndPaths[nextBeamL] = paths;
                    i = newBeamsAndPaths.find(nextBeamR);
                    if (i != newBeamsAndPaths.end())
                        i->second += paths;
                    else
                        newBeamsAndPaths[nextBeamR] = paths;
                } else {
                    Loc nextBeam = {std::get<0>(beam)+1, std::get<1>(beam)};
                    auto i = newBeamsAndPaths.find(nextBeam);
                    if (i != newBeamsAndPaths.end())
                        i->second += paths;
                    else
                        newBeamsAndPaths[nextBeam] = paths;
                }
            }
            beamsAndPaths = newBeamsAndPaths;
        }
        if (part == advhb::PuzzlePart::PartOne)
            return splits;
        std::uint64_t paths = 0;
        for (const auto& beamAndPath : beamsAndPaths)
            paths += std::get<1>(beamAndPath);
        return paths;
    }

    advhb::Solution s1(
        2025,
        7,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartOne))}; }
    );

    advhb::Solution s2(
        2025,
        7,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartTwo))}; }
    );
}
