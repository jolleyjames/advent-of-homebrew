#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
//DEBUG
#include <iostream>

#include "solution.hpp"

namespace y2025d02
{
    std::uint64_t sumInvalidIds(std::uint64_t start, std::uint64_t end) {
        if (start > end) {
            //throw std::invalid_argument("start greater than end");
            //DEBUG
            std::cout << "start " << start << " greater than end " << end << std::endl;
            return 0;
        }
        
        int startDigits = static_cast<int>(std::floor(std::log10(static_cast<double>(start)) + 1.0));
        if (startDigits % 2 == 1)
            startDigits++;
        int indexDigits = startDigits/2;
        std::uint64_t minIndex = 1;
        for (int i = 1; i < indexDigits; i++)
            minIndex *= 10;
        std::uint64_t index = start;
        std::uint64_t mult = 1;
        for (int i = 0; i < indexDigits; i++) {
            index /= 10;
            mult *= 10;
        }
        mult++;
        if (index < minIndex)
            index = minIndex;
        std::uint64_t incrIndexDigitsWhenIndex = minIndex * 10;
        std::uint64_t sum = 0;
        while (mult * index <= end) {
            if (mult * index >= start) {
                //DEBUG
                //std::cout << mult*index << std::endl;
                sum += (mult * index);
            }
            index++;
            if (index == incrIndexDigitsWhenIndex) {
                mult--;
                mult *= 10;
                mult++;
                incrIndexDigitsWhenIndex *= 10;
            }
        }
        return sum;
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

    std::uint64_t part1(std::ifstream &in) {
        //DEBUG
        std::cout << "sizeof(long) == " << sizeof(long) << std::endl;
        std::cout << "sizeof(long long) == " << sizeof(long long) << std::endl;
        //END DEBUG
        std::vector<std::uint64_t> ranges = loadRanges(in);
        std::uint64_t sum = 0;
        std::vector<std::uint64_t>::size_type i = 0;
        while (i < ranges.size()) {
            sum += sumInvalidIds(ranges[i], ranges[i+1]);
            i += 2;
        }
        return sum;
    }


    //TODO delete me
    std::vector<std::string> testLoad(std::ifstream &in) {
        std::vector<std::uint64_t> ranges = loadRanges(in);
        for (std::uint64_t range : ranges)
            std::cout << range << std::endl;
        std::vector<std::string> emptyVec;
        return emptyVec;
    }

    advhb::Solution s1(
        2025,
        2,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream &in){ return std::vector<std::string>{std::to_string(part1(in))}; }
    );


}
