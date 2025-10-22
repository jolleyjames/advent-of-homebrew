#include <vector>
#include <iostream>
#include <sstream>

#include "solution.hpp"

namespace y2023d02 {
    
    // a Reveal includes some number of red, green, and blue cubes.
    // a Game involves some number of Reveals.

    class Reveal {
    private:
        int red;
        int green;
        int blue;
    public:
        Reveal(int red, int green, int blue): red(red), green(green), blue(blue) {}
        Reveal(const Reveal& r): red(r.red), green(r.green), blue(r.blue) {}
        Reveal(Reveal&& r): red(r.red), green(r.green), blue(r.blue) {}
        int getRed() const {return red;}
        int getGreen() const {return green;}
        int getBlue() const {return blue;}
    };

    class Game {
    private:
        std::vector<Reveal> reveals;
    public:
        Game(const std::vector<Reveal>& reveals): reveals(reveals) {}
        Game(const Game& g): reveals(g.reveals) {}
        Game(Game&& g): reveals(g.reveals) {}
        const std::vector<Reveal>& getReveals() const {return reveals;}
        Reveal fewestCubes() const;
        bool possibleConfiguration(int red, int green, int blue) const;
    };

    Reveal Game::fewestCubes() const {
        int maxRed = 0;
        int maxGreen = 0;
        int maxBlue = 0;
        for (const Reveal& reveal : reveals) {
            if (reveal.getRed() > maxRed)
                maxRed = reveal.getRed();
            if (reveal.getGreen() > maxGreen)
                maxGreen = reveal.getGreen();
            if (reveal.getBlue() > maxBlue)
                maxBlue = reveal.getBlue();
        }
        return Reveal(maxRed, maxGreen, maxBlue);
    }

    bool Game::possibleConfiguration(int red, int green, int blue) const {
        Reveal max = fewestCubes();
        return red >= max.getRed() && green >= max.getGreen() && blue >= max.getBlue();
    }

    std::vector<Game> fileToGames(std::ifstream& in) {
        std::vector<Game> games;
        std::string line;
        while (std::getline(in, line)) {
            std::stringstream ss(line);
            // ignore first two entries: "Game", "##:"
            std::string _;
            ss >> _ >> _;
            // number of cubes shown and the cubes' color
            int count = 0;
            std::string color;
            // initialize color counts for each reveal
            int red = 0;
            int green = 0;
            int blue = 0;
            // reveals for this game
            std::vector<Reveal> reveals;

            while(true) {
                ss >> count >> color;
                if (color.find("red") != std::string::npos)
                    red = count;
                else if (color.find("green") != std::string::npos)
                    green = count;
                else
                    blue = count;
                if (color.back() != ',') {
                    reveals.push_back(Reveal(red, green, blue));
                    red = 0;
                    green = 0;
                    blue = 0;
                    if (color.back() != ';')
                        break;
                }
            }
            games.push_back(Game(reveals));
        }
        return games;
    }

    unsigned int part1(std::ifstream& in) {
        std::vector<Game> games = fileToGames(in);
        unsigned int total = 0;
        for (std::size_t i = 0; i < games.size(); i++) {
            if (games[i].possibleConfiguration(12, 13, 14))
                total += (i+1);
        }
        return total;
    }

    int part2(std::ifstream& in) {
        std::vector<Game> games = fileToGames(in);
        int total = 0;
        for (std::size_t i = 0; i < games.size(); i++) {
            Reveal r = games[i].fewestCubes();
            total += (r.getRed() * r.getGreen() * r.getBlue());
        }
        return total;
    }

    advhb::Solution s1 = advhb::Solution(
        2023,
        2, 
        advhb::PuzzlePart::PartOne, 
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part1(in))};}
    );

    advhb::Solution s2 = advhb::Solution(
        2023,
        2, 
        advhb::PuzzlePart::PartTwo, 
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part2(in))};}
    );

}