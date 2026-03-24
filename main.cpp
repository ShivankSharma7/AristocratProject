#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <memory>

// --- ANSI COLORS ---
#define RESET   "\033[0m"
#define GOLD    "\033[1;33m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"
#define CLEAR   "\033[2J\033[1;1H"

enum class Symbol { Cherry, Lemon, Bell, Seven, Jackpot };

struct SymbolData {
    std::string icon;
    std::string color;
    int multiplier;
};

class SlotMachine {
private:
    std::mt19937 rng;
    std::vector<Symbol> symbols = {Symbol::Cherry, Symbol::Lemon, Symbol::Bell, Symbol::Seven, Symbol::Jackpot};
    
    SymbolData getDetails(Symbol s) {
        switch(s) {
            case Symbol::Cherry:  return {"[CHERRY]", GREEN, 5};
            case Symbol::Lemon:   return {"[LEMON ]", CYAN, 10};
            case Symbol::Bell:    return {"[ BELL ]", YELLOW, 25};
            case Symbol::Seven:   return {"[SEVEN ]", RED, 80};
            case Symbol::Jackpot: return {"[JACKPT]", GOLD, 250};
            default: return {"[ ???  ]", RESET, 0};
        }
    }

public:
    SlotMachine() {
        rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    }

    void drawFrame(const std::vector<Symbol>& reels, int credits, int totalWins, int lastWin) {
        std::cout << CLEAR;
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << GOLD << "       ARISTOCRAT APEX-PRO v4.0         " << RESET << std::endl;
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << " CREDITS: " << GREEN << "$" << credits << RESET 
                  << " | TOTAL WINS: " << YELLOW << "$" << totalWins << RESET << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "\n       ";
        for(auto s : reels) {
            SymbolData d = getDetails(s);
            std::cout << d.color << d.icon << " " << RESET;
        }
        std::cout << "\n\n----------------------------------------" << std::endl;
        if (lastWin > 0) std::cout << GOLD << "   *** WINNER: $" << lastWin << " ***" << RESET << std::endl;
        else std::cout << "      SPIN TO WIN THE JACKPOT!       " << std::endl;
        std::cout << "========================================" << std::endl;
    }

    void spinAnimation(int credits, int totalWins) {
        for (int i = 0; i < 12; ++i) {
            std::vector<Symbol> temp;
            std::uniform_int_distribution<int> dist(0, (int)symbols.size() - 1);
            for(int j=0; j<3; j++) temp.push_back(symbols[dist(rng)]);
            drawFrame(temp, credits, totalWins, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + (i * 10)));
        }
    }

    std::vector<Symbol> getResult() {
        std::vector<Symbol> res;
        std::uniform_int_distribution<int> dist(0, (int)symbols.size() - 1);
        for(int i=0; i<3; i++) res.push_back(symbols[dist(rng)]);
        return res;
    }

    int evaluate(const std::vector<Symbol>& res, int bet) {
        if (res[0] == res[1] && res[1] == res[2]) return bet * getDetails(res[0]).multiplier;
        return 0;
    }
};

void printVoucher(int totalWins, int finalBalance) {
    std::cout << CLEAR;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "|         CASHOUT VOUCHER          |" << std::endl;
    std::cout << "|      ARISTOCRAT TECH DEMO        |" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "| SESSION WINS:    $" << totalWins << std::endl;
    std::cout << "| TOTAL PAYOUT:    $" << finalBalance << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "|  VALID FOR 30 DAYS AT CAGE ONLY  |" << std::endl;
    std::cout << "------------------------------------" << std::endl;
}

int main() {
    SlotMachine game;
    int credits = 0;
    int totalSessionWins = 0;
    int lastWin = 0;

    std::cout << RED << "WARNING: UNREDEEMED CREDITS FORFEITED ON EXIT." << RESET << std::endl;
    std::cout << "Enter Starting Credits: $";
    std::cin >> credits;
    std::string input;
    std::getline(std::cin, input); // clear buffer

    bool running = true;
    while (credits >= 10 && running) {
        game.drawFrame({Symbol::Cherry, Symbol::Bell, Symbol::Seven}, credits, totalSessionWins, lastWin);
        std::cout << "\n[ENTER] Spin ($10) | [C] Cash Out: ";
        std::getline(std::cin, input);

        if (input == "c" || input == "C") {
            if (totalSessionWins > 0) {
                printVoucher(totalSessionWins, credits);
                running = false;
            } else {
                std::cout << RED << "No wins recorded. Forfeit remaining $" << credits << "? (y/n): " << RESET;
                std::getline(std::cin, input);
                if (input == "y" || input == "Y") running = false;
            }
            continue;
        }

        credits -= 10;
        game.spinAnimation(credits, totalSessionWins);
        auto res = game.getResult();
        lastWin = game.evaluate(res, 10);
        credits += lastWin;
        totalSessionWins += lastWin;
        game.drawFrame(res, credits, totalSessionWins, lastWin);
        
        std::cout << "\nPress ENTER to continue...";
        std::getline(std::cin, input);
    }

    if (credits < 10 && running) std::cout << RED << "\nOUT OF CREDITS." << RESET << std::endl;
    return 0;
}
