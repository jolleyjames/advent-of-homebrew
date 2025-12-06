#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
#include <set>

#include "solution.hpp"

namespace y2025d02
{

    std::uint64_t findMult(const int repeats, const int indexDigits) {
        std::uint64_t mult = 1;
        for (int j = 0; j < repeats - 1; j++) {
            for (int k = 0; k < indexDigits; k++) {
                mult *= 10;
            }
            mult++;
        }
        return mult;
    }


    void findInvalidIds(std::set<std::uint64_t>& ids, const std::uint64_t start, const std::uint64_t end, const int repeats) {
        if (start > end)
            throw std::invalid_argument("start greater than end");
        int startDigits = static_cast<int>(std::floor(std::log10(static_cast<double>(start)) + 1.0));
        if (startDigits % repeats != 0)
            startDigits += (repeats - (startDigits % repeats));
        int indexDigits = startDigits/repeats;
        std::uint64_t minIndex = 1;
        for (int i = 1; i < indexDigits; i++)
            minIndex *= 10;
        std::uint64_t index = start;
        for (int i = 0; i < (repeats-1)*indexDigits; i++)
            index /= 10;
        std::uint64_t mult = findMult(repeats, indexDigits);
        if (index < minIndex)
            index = minIndex;
        std::uint64_t incrIndexDigitsWhenIndex = minIndex * 10;
        while (mult * index <= end) {
            if (mult * index >= start) {
                ids.insert(mult * index);
            }
            index++;
            if (index == incrIndexDigitsWhenIndex) {
                indexDigits++;
                mult = findMult(repeats, indexDigits);
                incrIndexDigitsWhenIndex *= 10;
            }
        }
    }

    std::vector<std::uint64_t> loadRanges(std::ifstream &in) {
        std::string rangeString;
        std::getline(in, rangeString);
        std::vector<std::uint64_t> ranges;
        char nextDelim = '-';
        std::string::size_type from = 0;
        std::string::size_type to = rangeString.find(nextDelim, from);
        while (from != std::string::npos) {
            to = rangeString.find(nextDelim, from);
            std::uint64_t next = static_cast<std::uint64_t>(std::stoll(rangeString.substr(from, to-from)));
            ranges.push_back(next);
            if (nextDelim == '-')
                nextDelim = ',';
            else
                nextDelim = '-';
            if (to == std::string::npos)
                from = to;
            else {
                from = to+1;
                to = rangeString.find(nextDelim, from);
            }
        } while (to != std::string::npos);
        return ranges;
    }

    std::uint64_t generalized(std::ifstream &in, advhb::PuzzlePart part) {
        std::vector<std::uint64_t> ranges = loadRanges(in);
        std::set<std::uint64_t> ids;
        // How many digits does the largest value in all ranges have?
        std::uint64_t maxRange = 0;
        for (std::uint64_t range : ranges)
            if (range > maxRange)
                maxRange = range;
        int maxDigits = static_cast<int>(std::floor(std::log10(static_cast<double>(maxRange)) + 1.0));
        int primes[] = {2,3,5,7,11,13,17};
        for (unsigned int j = 0; j < sizeof(primes)/sizeof(int) && primes[j] <= maxDigits; j++) {
            std::vector<std::uint64_t>::size_type i = 0;
            while (i < ranges.size()) {
                findInvalidIds(ids, ranges[i], ranges[i+1], primes[j]);
                i += 2;
            }
            if (part == advhb::PuzzlePart::PartOne)
                break;
        }
        std::uint64_t sum = 0;
        for (std::uint64_t id : ids)
            sum += id;
        return sum;
    }

    advhb::Solution s1(
        2025,
        2,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartOne))}; }
    );

    advhb::Solution s2(
        2025,
        2,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(generalized(in, advhb::PuzzlePart::PartTwo))}; }
    );

}
