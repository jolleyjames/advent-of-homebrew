#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <tuple>
#include <iterator>

#include "solution.hpp"

namespace y2025d05
{
    class Range {
        private:
        std::int64_t _first;
        std::int64_t _last;

        public:
        Range(std::int64_t first, std::int64_t last) {
            if (first > last)
                throw std::invalid_argument("invalid range");
            _first = first;
            _last = last;
        }
        std::int64_t getFirst() const {return _first;}
        std::int64_t getLast() const {return _last;}
        bool operator<(const Range& rhs) const {
            return _first < rhs._first || (_first == rhs._first && _last < rhs._last);
        }
        bool tryReduce(const Range& gteRange) {
            //undefined behavior if gteRange not >= this
            if (_last < gteRange._first - 1)
                return false;
            _last = std::max(_last, gteRange._last);
            return true;
        }
    };

    std::tuple<std::list<Range>,std::vector<std::int64_t>> loadIngredientDb(std::ifstream &in) {
        std::string line;
        std::list<Range> ranges;
        std::vector<std::int64_t> ingredients;
        bool loadingRanges = true;
        while (std::getline(in, line)) {
            if (loadingRanges) {
                if (line.empty())
                    loadingRanges = false;
                else {
                    std::string::size_type dash = line.find('-');
                    std::int64_t first = std::stoll(line.substr(0,dash));
                    std::int64_t last = std::stoll(line.substr(dash+1));
                    ranges.push_back(Range(first,last));
                }
            } else {
                std::int64_t ingredient = std::stoll(line);
                ingredients.push_back(ingredient);
            }
        }
        // sort and reduce ranges
        ranges.sort();
        auto itr = ranges.begin();
        while (std::next(itr) != ranges.end()) {
            if (itr->tryReduce(*std::next(itr))) {
                ranges.erase(std::next(itr));
            } else {
                itr++;
            }
        }
        return {ranges, ingredients};
    }

    std::size_t countFresh(std::ifstream &in) {
        auto db = loadIngredientDb(in);
        auto ranges = std::get<0>(db);
        auto ingredients = std::get<1>(db);
        std::size_t fresh = 0;
        for (auto ingredient : ingredients) {
            for (const auto& range : ranges) {
                if (range.getFirst() <= ingredient && ingredient <= range.getLast()) {
                    fresh++;
                    break;
                } else if (ingredient <= range.getFirst())
                    break;
            }
        }
        return fresh;
    }

    std::int64_t totalFresh(std::ifstream &in) {
        auto db = loadIngredientDb(in);
        auto ranges = std::get<0>(db);
        std::int64_t fresh = 0;
        for (const auto& range : ranges) {
            fresh += (range.getLast() - range.getFirst() + 1);
        }
        return fresh;
    }

    advhb::Solution s1(
        2025,
        5,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(countFresh(in))}; }
    );

    advhb::Solution s2(
        2025,
        5,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(totalFresh(in))}; }
    );
}
