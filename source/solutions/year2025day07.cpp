#include <fstream>
#include <string>
#include <set>
#include <tuple>
#include <cstdint>

#include <sstream>
#include <vector>
#include <iostream>

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

    int part1(std::ifstream &in) {
        auto diagram = loadDiagram(in);
        Loc start = std::get<0>(diagram);
        std::set<Loc> splitters = std::get<1>(diagram);
        std::size_t yEndAt = std::get<2>(diagram);
        std::set<Loc> beams = {start};
        std::size_t splits = 0;
        for (std::size_t y = 0; y < yEndAt; y++) {
            std::set<Loc> beamsAdded, beamsRemoved;
            for (const auto& beam : beams) {
                if (splitters.find(beam) != splitters.end()) {
                    beamsAdded.insert({std::get<0>(beam)+1, std::get<1>(beam)-1});
                    beamsAdded.insert({std::get<0>(beam)+1, std::get<1>(beam)+1});
                    beamsRemoved.insert({std::get<0>(beam)+1, std::get<1>(beam)});
                    splits++;
                }
            }
            std::set<Loc> newBeams;
            for (const auto& beam : beams) {
                Loc newBeam = {std::get<0>(beam)+1, std::get<1>(beam)};
                if (beamsRemoved.find(newBeam) == beamsRemoved.end())
                    newBeams.insert(newBeam);
                newBeams.insert(beamsAdded.begin(), beamsAdded.end());
            }
            beams = newBeams;
        }
        return splits;
    }

    advhb::Solution s1(
        2025,
        7,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );



}
