#include <vector>
#include <string>
#include <cctype>
#include <tuple>
#include <map>
#include <fstream>

#include "solution.hpp"

namespace y2023d03 {

    class NumberLocation {
    private:
        std::size_t row;
        std::size_t firstCol;
        std::size_t lastCol;
    public:
        NumberLocation(std::size_t row, std::size_t firstCol, std::size_t lastCol):
          row(row), firstCol(firstCol), lastCol(lastCol) {}
        NumberLocation(const NumberLocation& pn):
          row(pn.row), firstCol(pn.firstCol), lastCol(pn.lastCol) {}
        NumberLocation(NumberLocation&& pn):
          row(pn.row), firstCol(pn.firstCol), lastCol(pn.lastCol) {}
        std::size_t getRow() const {return row;}
        std::size_t getFirstCol() const {return firstCol;}
        std::size_t getLastCol() const {return lastCol;}
    };

    class Schematic {
    private:
        std::vector<std::string> rows;
        std::vector<NumberLocation> partNumbers;
        std::map<std::tuple<std::size_t, std::size_t>, char> symbols;
        void scan();
    public:
        Schematic(const std::vector<std::string>& rows): rows(rows) {scan();}
        Schematic(const Schematic& s): rows(s.rows) {scan();}
        Schematic(Schematic&& s): rows(s.rows) {scan();}
        const std::vector<std::string>& getRows() const {return rows;}
        const std::vector<NumberLocation>& getPartNumbers() const {return partNumbers;}
        const std::map<std::tuple<std::size_t, std::size_t>, char>& getSymbols() const {return symbols;}
    };

    void Schematic::scan() {
        for (size_t r = 0; r < rows.size(); r++) {
            bool scanningNumber = false;
            size_t firstNumberCol;
            size_t lastNumberCol;
            for (size_t c = 0; c < rows[r].size(); c++) {
                if (std::isdigit(rows[r][c])) {
                    if (!scanningNumber) {
                        scanningNumber = true;
                        firstNumberCol = c;
                    }
                    lastNumberCol = c;
                } else {
                    if (scanningNumber) {
                        scanningNumber = false;
                        partNumbers.push_back(NumberLocation(r, firstNumberCol, lastNumberCol));
                    }
                    if (rows[r][c] != '.')
                        symbols[{r,c}] = rows[r][c];                        
                }
            }
            if (scanningNumber)
                partNumbers.push_back(NumberLocation(r, firstNumberCol, lastNumberCol));
        }
    }

    int getValue(const NumberLocation& nl, const Schematic& s) {
        int value = 0;
        for (std::size_t i = nl.getFirstCol(); i <= nl.getLastCol(); i++) {
            value *= 10;
            value += (int)(s.getRows()[nl.getRow()][i] - '0');
        }
        return value;
    }

    bool isPartNumber(const NumberLocation& nl, const Schematic& s) {
        for (std::size_t r = (nl.getRow() == 0)? 0 : nl.getRow()-1; r <= nl.getRow()+1; r++) {
            if (r == nl.getRow()) {
                if (s.getSymbols().find({nl.getRow(), nl.getFirstCol()-1}) != s.getSymbols().cend() ||
                    s.getSymbols().find({nl.getRow(), nl.getLastCol()+1}) != s.getSymbols().cend())
                    return true;
            } else for (std::size_t c = (nl.getFirstCol() == 0)? 0 : nl.getFirstCol()-1; c <= nl.getLastCol()+1; c++) {
                if (s.getSymbols().find({r,c}) != s.getSymbols().cend())
                    return true;
            }
        }
        return false;
    }

    std::vector<std::string> loadRows(std::ifstream& in) {
        std::vector<std::string> rows;
        std::string row;
        while(std::getline(in, row))
            rows.push_back(row);
        return rows;
    }

    int part1(std::ifstream& in) {
        Schematic schem(loadRows(in));
        int sum = 0;
        for (const NumberLocation& nl : schem.getPartNumbers()) {
            if (isPartNumber(nl, schem))
                sum += getValue(nl, schem);
        }
        return sum;
    }

    void gearScan(const NumberLocation& nl, const Schematic& s, std::map<std::tuple<std::size_t, std::size_t>, std::vector<const NumberLocation*> >& potentialGears) {
        for (std::size_t r = (nl.getRow() == 0)? 0 : nl.getRow()-1; r <= nl.getRow()+1 && r < s.getRows().size(); r++) {
            for (std::size_t c = (nl.getFirstCol() == 0)? 0 : nl.getFirstCol()-1; c <= nl.getLastCol()+1 && c < s.getRows()[r].size(); c++) {
                if (s.getRows()[r][c] == '*') {
                    if (potentialGears.find({r,c}) == potentialGears.end())
                        potentialGears[{r,c}] = std::vector<const NumberLocation*>();
                    potentialGears[{r,c}].push_back(&nl);
                }
            }
        }
    }

    int part2(std::ifstream& in) {
        Schematic schem(loadRows(in));
        std::map<std::tuple<std::size_t, std::size_t>, std::vector<const NumberLocation*> > potentialGears;
        for (const NumberLocation& nl : schem.getPartNumbers())
            gearScan(nl, schem, potentialGears);
        int sum = 0;
        for (const auto& potentialGear : potentialGears) {
            if (potentialGear.second.size() == 2)
                sum += (getValue(*(potentialGear.second[0]), schem) * getValue(*(potentialGear.second[1]), schem));
        }
        return sum;
    }

    advhb::Solution s1(
        2023,
        3,
        advhb::PuzzlePart::PartOne,
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part1(in))};}
    );

    advhb::Solution s2(
        2023,
        3,
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream& in){return std::vector<std::string>{std::to_string(part2(in))};}
    );

}
