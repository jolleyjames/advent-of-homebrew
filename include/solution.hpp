#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace advhb {
    enum class PuzzlePart { PartOne, PartTwo };

    class Solution {
    private:
        int year;
        int day;
        PuzzlePart part;
        std::function<std::vector<std::string>(std::ifstream&)> invokeFunction;
        static std::map<std::tuple<int, int, PuzzlePart>, Solution*> solutionsMap;
        static const std::filesystem::path APP_SD_PATH;

    public:
        Solution(int, int, PuzzlePart, std::function<std::vector<std::string>(std::ifstream&)>);
        ~Solution();
        typedef std::tuple<int, int, PuzzlePart> Key;
        int getYear() const;
        int getDay() const;
        PuzzlePart getPart() const;
        std::vector<std::string> invoke(bool test=false) const;
        static const std::map<Key, Solution*>& getSolutions();
    };
}

#endif /* SOLUTION_HPP */
