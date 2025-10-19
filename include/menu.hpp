#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>
#include <set>
#include <map>
#include <string>

#include "solution.hpp"

namespace advhb {
    enum class MenuState { Uninitialized, YearEntry, DayEntry, PartEntry, TestInputEntry, RunSolution, SolutionFinished, Exit, Error };

    std::ostream& operator<<(std::ostream&, MenuState);

    class Menu {
    private:
        // singleton pattern
        Menu();
        Menu(const Menu&) = delete;
        Menu(Menu&&) = delete;
        Menu& operator=(const Menu&) = delete;
        Menu& operator=(Menu&&) = delete;

        MenuState state = MenuState::Uninitialized;
        std::map<Solution::Key, Solution*> solutionsMap;
        std::map<int, std::set<int> > yearDayOpts;
        std::string errorMessage;
        int selectedYear;
        int selectedDay;
        PuzzlePart selectedPart;
        bool useTestInput;
        bool continueButtonScan = true;
        bool powerPressed = false;
        bool resetPressed = false;
        void init();
        int selectYear();
        int selectDay();
        PuzzlePart selectPart();
        bool chooseTestInput();
        void runSolution();
        void postSolution();
        void exitApp();
        void error();
    public:
        static Menu& getInstance();
        void setSolutions(const std::map<Solution::Key, Solution*>&);
        void setErrorMessage(const std::string&);
        void next();
        void power();
        void reset();
    };
}

#endif /* MENU_HPP */
