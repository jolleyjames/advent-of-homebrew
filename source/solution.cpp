#include <sstream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "solution.hpp"

advhb::Solution::Solution(int year, int day, PuzzlePart part, std::function<std::vector<std::string>(std::ifstream&)> invokeFunction) : 
  year(year), day(day), part(part), invokeFunction(invokeFunction) {
    solutionsMap[{year, day, part}] = this;
}

advhb::Solution::~Solution() {
    advhb::Solution::Key key = {this->year, this->day, this->part};
    solutionsMap.erase(key);
}

int advhb::Solution::getYear() const {
    return year;
}

int advhb::Solution::getDay() const {
    return day;
}

advhb::PuzzlePart advhb::Solution::getPart() const {
    return part;
}

std::vector<std::string> advhb::Solution::invoke(bool test) const {
    std::filesystem::path textInputPath = APP_SD_PATH;
    // Input file name assumed to be "YYYY-DD.txt"
    // Regular inputs in directory "input"; test inputs in directory "input-test".
    textInputPath /= (test? "input-test" : "input");
    std::stringstream filenameStream;
    filenameStream << std::setfill('0') << std::setw(4) << getYear() << "-" << std::setw(2) << getDay() << ".txt";
    textInputPath /= filenameStream.str();
    std::ifstream infile;
    infile.open(textInputPath);
    if (!infile.is_open() && test) {
        // If test file is missing, look for "YYYY-DD.txt.1" or "YYYY-DD.txt.2"
        std::filesystem::path fallbackPath = textInputPath;
        fallbackPath += ".";
        fallbackPath += (part == advhb::PuzzlePart::PartOne)? "1" : "2";
        infile.open(fallbackPath);
    }
    if (!infile.is_open()) {
        std::stringstream errorMsgStream;
        errorMsgStream << "ERROR: could not open " << textInputPath;
        return {errorMsgStream.str()};
    }

    auto result = invokeFunction(infile);
    if (infile.is_open())
        infile.close();
    return result;
}

const std::map<advhb::Solution::Key, advhb::Solution*>& advhb::Solution::getSolutions() {
    return solutionsMap;
}

std::map<advhb::Solution::Key, advhb::Solution*> advhb::Solution::solutionsMap;

const std::filesystem::path advhb::Solution::APP_SD_PATH = std::filesystem::path("/apps/advhb");
