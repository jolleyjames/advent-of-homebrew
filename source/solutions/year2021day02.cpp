#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <fstream>

#include "solution.hpp"

namespace y2021d02 {

    enum class InstructionDirection { Forward, Up, Down };

    class Instruction {
    private:
        InstructionDirection direction;
        int value;
    public:
        Instruction(InstructionDirection direction, int value) : direction(direction), value(value) {}
        Instruction(const Instruction& instr) : direction(instr.getDirection()), value(instr.getValue()) {}
        Instruction(Instruction&& instr) : direction(instr.getDirection()), value(instr.getValue()) {}
        Instruction(const std::string& s) {
            std::stringstream ss(s);
            std::string directionStr;
            ss >> directionStr;
            if (directionStr == "forward")
                direction = InstructionDirection::Forward;
            else if (directionStr == "up")
                direction = InstructionDirection::Up;
            else if (directionStr == "down")
                direction = InstructionDirection::Down;
            else
                throw std::runtime_error("illegal direction " + directionStr);
            ss >> value;
        }
        InstructionDirection getDirection() const {return direction;}
        int getValue() const {return value;}
    };

    std::ostream& operator<<(std::ostream &os, Instruction i) {
        os << "(";
        switch (i.getDirection()) {
        case InstructionDirection::Forward:
            os << "forward";
            break;
        case InstructionDirection::Up:
            os << "up";
            break;
        case InstructionDirection::Down:
            os << "down";
            break;
        default:
            throw std::logic_error("bug: not all InstructionDirection values handled");
        }
        os << ", " << i.getValue() << ")";
        return os;
    }

    class Position {
    protected:
        int horz;
        int depth;
    public:
        Position() : horz(0), depth(0) {}
        Position(const Position& p) : horz(p.getHorz()), depth(p.getDepth()) {}
        Position(Position&& p) : horz(p.getHorz()), depth(p.getDepth()) {}
        int getHorz() const {return horz;}
        int getDepth() const {return depth;}
        virtual void apply(const Instruction& instr) {
            switch (instr.getDirection()) {
            case InstructionDirection::Forward:
                horz += instr.getValue();
                break;
            case InstructionDirection::Up:
                depth -= instr.getValue();
                break;
            case InstructionDirection::Down:
                depth += instr.getValue();
                break;
            default:
                throw std::logic_error("bug: not all InstructionDirection values handled");
            }
        }
    };

    class AimingPosition : public Position {
    private:
        int aim;
    public:
        AimingPosition() : Position(), aim(0) {}
        AimingPosition(const AimingPosition& p) : Position(p), aim(p.getAim()) {}
        AimingPosition(AimingPosition&& p) : Position(p), aim(p.getAim()) {}
        int getAim() const {return aim;}
        void apply(const Instruction& instr) {
            switch (instr.getDirection()) {
            case InstructionDirection::Forward:
                horz += instr.getValue();
                depth += (aim * instr.getValue());
                break;
            case InstructionDirection::Up:
                aim -= instr.getValue();
                break;
            case InstructionDirection::Down:
                aim += instr.getValue();
                break;
            default:
                throw std::logic_error("bug: not all InstructionDirection values handled");
            }
        }

    };

    std::vector<Instruction> loadInstructions(std::ifstream& in) {
        std::vector<Instruction> v;
        std::string line = "";
        while (std::getline(in, line))
            v.push_back(Instruction(line));
        in.close();
        return v;
    }

    int generalized(std::ifstream& in, Position& p) {
        auto instructions = loadInstructions(in);
        for (auto instruction : instructions)
            p.apply(instruction);
        return p.getDepth() * p.getHorz();
    }

    advhb::Solution s1(
        2021, 
        2, 
        advhb::PuzzlePart::PartOne,
        [](std::ifstream& in) {
            Position p;
            return std::vector<std::string>{std::to_string(generalized(in,p))};
        }
    );

    advhb::Solution s2(
        2021, 
        2, 
        advhb::PuzzlePart::PartTwo,
        [](std::ifstream& in) {
            AimingPosition p;
            return std::vector<std::string>{std::to_string(generalized(in,p))};
        }
    );
}
