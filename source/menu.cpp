#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>

#include <gccore.h>
#include <wiiuse/wpad.h>

#include "menu.hpp"
#include "solution.hpp"

advhb::Menu::Menu() {
    init();
}

advhb::Menu& advhb::Menu::getInstance() {
    static Menu instance;
    return instance;
}

void advhb::Menu::setSolutions(const std::vector<Solution*>& solutions) {
    solutionsMap.clear();
    for (Solution* soln : solutions) {
        auto yritr = solutionsMap.find(soln->getYear());
        if (yritr == solutionsMap.end()) {
            std::map<int, std::map<advhb::PuzzlePart, Solution*> > dayMap = { {soln->getDay(), {{soln->getPart(), soln}} } };
            solutionsMap[soln->getYear()] = dayMap;
        } else {
            auto dayitr = yritr->second.find(soln->getDay());
            if (dayitr == yritr->second.end()) {
                std::map<advhb::PuzzlePart, Solution*> partMap = {{soln->getPart(), soln}};
                yritr->second[soln->getDay()] = partMap;
            } else {
                dayitr->second[soln->getPart()] = soln;
            }
        }
    }
    state = MenuState::YearEntry;
}

void advhb::Menu::setErrorMessage(const std::string& errorMessage) {
    this->errorMessage = errorMessage;
    this->state = MenuState::Error;
}

void advhb::Menu::next() {
    switch (state) {
    case MenuState::Uninitialized:
        throw std::runtime_error("Menu uninitialized, call setSolutions() or setErrorMessage()");
        break;
    case MenuState::YearEntry:
        selectedYear = selectYear();
        break;
    case MenuState::DayEntry:
        selectedDay = selectDay();
        break;
    case MenuState::PartEntry:
        selectedPart = selectPart();
        break;
    case MenuState::TestInputEntry:
        useTestInput = chooseTestInput();
        break;
    case MenuState::RunSolution:
        runSolution();
        break;
    case MenuState::SolutionFinished:
        postSolution();
        break;
    case MenuState::Exit:
        exitApp();
        break;
    case MenuState::Error:
        error();
        break;
    default:
        std::cout << "Bug in Menu::next()" << std::endl;
        break;    
    }
}

int advhb::Menu::selectYear() {
    std::map<int, std::map<int, std::map<PuzzlePart, Solution*> > >::const_iterator selection = solutionsMap.find(selectedYear);
    if (selection == solutionsMap.cend())
        selection = solutionsMap.cbegin();
    // clear screen, move cursor to top, prompt for year
    std::cout << "\033[2J";
    std::cout << "\x1b[2;0H";
    std::cout << "up/down: cycle options | A: select | B: back" << std::endl;
    std::cout << "Select year: " << selection->first << std::flush;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // dpad-up ==> previous; dpad-down ==> next; A ==> choose
        if (continueButtonScan && (pressed & WPAD_BUTTON_UP)) {
            if (selection == solutionsMap.cbegin())
                selection = solutionsMap.cend();
            selection--;
            std::cout << "\rSelect year: " << selection->first << std::flush;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_DOWN)) {
            selection++;
            if (selection == solutionsMap.cend())
                selection = solutionsMap.cbegin();
            std::cout << "\rSelect year: " << selection->first << std::flush;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_A)) {
            std::cout << std::endl;
            state = MenuState::DayEntry;
            return selection->first;
        }

        VIDEO_WaitVSync();
    }
    return 0;
}

int advhb::Menu::selectDay() {
    std::map<int, std::map<PuzzlePart, Solution*> >::const_iterator selection = solutionsMap[selectedYear].find(selectedDay);
    if (selection == solutionsMap[selectedYear].cend())
        selection = solutionsMap[selectedYear].cbegin();
    // clear screen, move cursor to top, show previously selected year, prompt for day
    std::cout << "\033[2J";
    std::cout << "\x1b[2;0H";
    std::cout << "up/down: cycle options | A: select | B: back" << std::endl;
    std::cout << "Select year: " << selectedYear << std::endl;
    std::cout << "Select day: " << selection->first << std::flush;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // dpad-up ==> previous; dpad-down ==> next; B ==> back to year; A ==> choose
        if (continueButtonScan && (pressed & WPAD_BUTTON_UP)) {
            if (selection == solutionsMap[selectedYear].cbegin())
                selection = solutionsMap[selectedYear].cend();
            selection--;
            std::cout << "\rSelect day: " << selection->first << std::flush;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_DOWN)) {
            selection++;
            if (selection == solutionsMap[selectedYear].cend())
                selection = solutionsMap[selectedYear].cbegin();
            std::cout << "\rSelect day: " << selection->first << std::flush;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_B)) {
            state = MenuState::YearEntry;
            return 0;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_A)) {
            std::cout << std::endl;
            state = MenuState::PartEntry;
            return selection->first;
        }

        VIDEO_WaitVSync();
    }
    return 0;
}

advhb::PuzzlePart advhb::Menu::selectPart() {
    // clear screen, move cursor to top, show previously selected year and day, prompt for puzzle part
    std::cout << "\033[2J";
    std::cout << "\x1b[2;0H";
    std::cout << "up/down: cycle options | A: select | B: back" << std::endl;
    std::cout << "Select year: " << selectedYear << std::endl;
    std::cout << "Select day: " << selectedDay << std::endl;
    std::cout << "Press 1 or 2 to select part 1 or 2: " << std::flush;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // dpad-up ==> previous; dpad-down ==> next; B ==> back to year; A ==> choose
        if (continueButtonScan && (pressed & WPAD_BUTTON_1)) {
            state = MenuState::TestInputEntry;
            return PuzzlePart::PartOne;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_2)) {
            state = MenuState::TestInputEntry;
            return PuzzlePart::PartTwo;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_B)) {
            state = MenuState::DayEntry;
            return PuzzlePart::PartOne;
        }

        VIDEO_WaitVSync();
    }
    return PuzzlePart::PartOne;
}

bool advhb::Menu::chooseTestInput() {
    bool testInput = false;
    // clear screen, move cursor to top, show previously selected year day and part, prompt for test input
    std::cout << "\033[2J";
    std::cout << "\x1b[2;0H";
    std::cout << "up/down: cycle options | A: select | B: back" << std::endl;
    std::cout << "Select year: " << selectedYear << std::endl;
    std::cout << "Select day: " << selectedDay << std::endl;
    std::cout << "Press 1 or 2 to select part 1 or 2: " << (selectedPart == PuzzlePart::PartOne? "1" : "2") << std::endl;
    std::cout << "Use test input: " << (testInput? "yes" : "no ") << std::flush;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // dpad-up ==> previous; dpad-down ==> next; B ==> back to part choice; A ==> choose
        if (continueButtonScan && (pressed & (WPAD_BUTTON_UP | WPAD_BUTTON_DOWN))) {
            testInput = !testInput;
            std::cout << "\rUse test input: " << (testInput? "yes" : "no ") << std::flush;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_B)) {
            state = MenuState::PartEntry;
            return false;
        }
        else if (continueButtonScan && (pressed & WPAD_BUTTON_A)) {
            std::cout << std::endl;
            state = MenuState::RunSolution;
            return testInput;
        }

        VIDEO_WaitVSync();
    }
    return false;
}

void advhb::Menu::runSolution() {
    auto solnitr = solutionsMap[selectedYear][selectedDay].find(selectedPart);
    if (solnitr == solutionsMap[selectedYear][selectedDay].end()) {
        std::cout << "ERROR: Part " << (selectedPart == advhb::PuzzlePart::PartOne? "1" : "2") << " solution not found" << std::endl;
    } else {
        auto result = solnitr->second->invoke(useTestInput);
        for (auto line: result)
            std::cout << line << std::endl;
    }
    state = MenuState::SolutionFinished;
}

void advhb::Menu::postSolution() {
    std::cout << "Press A to choose another solution, HOME to exit" << std::endl;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // A ==> select year
        // Home ==> exit
        if (continueButtonScan && (pressed & WPAD_BUTTON_A)) {
            state = MenuState::YearEntry;
            break;
        } else if (continueButtonScan && (pressed & WPAD_BUTTON_HOME)) {
            state = MenuState::Exit;
            break;
        }
            
        VIDEO_WaitVSync();
    }
}

void advhb::Menu::exitApp() {
    // unregister callbacks
    SYS_SetResetCallback(nullptr);
    SYS_SetPowerCallback(nullptr);
    WPAD_SetPowerButtonCallback(nullptr);

    const char* message;
    s32 resetType;
    if (powerPressed) {
        std::cout << std::endl;
        message = "powering off Wii...";
        resetType = SYS_POWEROFF_STANDBY;
    } else {
        if (resetPressed)
            std::cout << std::endl;
        message = "returning to Wii menu...";
        resetType = SYS_RETURNTOMENU;
    }
	std::cout << message << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	SYS_ResetSystem(resetType, 0, 0);
}

void advhb::Menu::error() {
    std::cout << errorMessage << std::endl;
    std::cout << "Press HOME to exit" << std::endl;
    while (continueButtonScan) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        if (continueButtonScan && (pressed & WPAD_BUTTON_HOME)) {
            state = MenuState::Exit;
            break;
        }
            
        VIDEO_WaitVSync();
    }
}

void resetCallback(u32 irq, void* ctx) {
    advhb::Menu& menu = advhb::Menu::getInstance();
    menu.reset();
}

void advhb::Menu::reset() {
    continueButtonScan = false;
    resetPressed = true;
    state = MenuState::Exit;
}

void powerCallback() {
    advhb::Menu& menu = advhb::Menu::getInstance();
    menu.power();
}

void wiimotePowerCallback(s32 chan) {
    advhb::Menu& menu = advhb::Menu::getInstance();
    menu.power();
}

void advhb::Menu::power() {
    continueButtonScan = false;
    powerPressed = true;
    state = MenuState::Exit;
}

void advhb::Menu::init() {
    // register callbacks for reset, power buttons
    SYS_SetResetCallback(resetCallback);
    SYS_SetPowerCallback(powerCallback);
    WPAD_SetPowerButtonCallback(wiimotePowerCallback);
}

std::ostream &advhb::operator<<(std::ostream &os, MenuState ms)
{
    switch (ms)
    {
    case MenuState::Uninitialized:
        os << "Uninitialized";
        break;
    case MenuState::YearEntry:
        os << "YearEntry";
        break;
    case MenuState::DayEntry:
        os << "DayEntry";
        break;
    case MenuState::PartEntry:
        os << "PartEntry";
        break;
    case MenuState::TestInputEntry:
        os << "TestInputEntry";
        break;
    case MenuState::RunSolution:
        os << "RunSolution";
        break;
    case MenuState::SolutionFinished:
        os << "SolutionFinished";
        break;
    case MenuState::Exit:
        os << "Exit";
        break;
    case MenuState::Error:
        os << "Error";
        break;
    default:
        os << "Bug in MenuState << operator";
        break;
    }
    return os;
}
