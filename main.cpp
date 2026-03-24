#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>

// --- ANSI Styling ---
#define RESET   "\033[0m"
#define GOLD    "\033[1;33m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define CLEAR   "\033[2J\033[1;1H"

enum class Symbol { Cherry, Lemon, Bell, Seven, Jackpot };

struct SymbolData {
    std::string icon;
    int multiplier;
};

class SlotEngine {
private:
    std::mt19937 rng;
    SymbolData getDetails(Symbol s) {
        switch(s) {
            case Symbol::Cherry:  return {"[ CHERRY ]", 5};
            case Symbol::Lemon:   return {"[ LEMON  ]", 10};
            case Symbol::Bell:    return {"[  BELL  ]", 40};
            case Symbol::Seven:   return {"[  SEVEN ]", 150};
            case Symbol::Jackpot: return {"[ JACKPOT ]", 1000};
            default: return {"[  ???   ]", 0};
        }
    }

public:
    SlotEngine() {
        rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    }

    void drawHeader(int credits, int totalWins, int currentBet, bool isAllIn) {
        std::cout << CLEAR;
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << GOLD << "       ARISTOCRAT APEX-PRO v8.0         " << RESET << std::endl;
        std::cout << GOLD << "       MADE BY SHIVANK SHARMA           " << RESET << std::endl; // NAME ADDED
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << " BANKROLL: " << GREEN << "$" << credits << RESET 
                  << " | TOTAL WINS: " << YELLOW << "$" << totalWins << RESET << std::endl;
        
        std::cout << " CURRENT WAGER: " << RED;
        if(isAllIn) std::cout << "ALL-IN ($" << currentBet << ")";
        else std::cout << "$" << currentBet;
        std::cout << RESET << std::endl;
        
        std::cout << "----------------------------------------" << std::endl;
    }

    void spinAnimation(int credits, int totalWins, int currentBet, bool isAllIn) {
        for (int i = 0; i < 12; ++i) {
            std::vector<Symbol> temp;
            std::uniform_int_distribution<int> dist(0, 4);
            for(int j=0; j<3; j++) temp.push_back(static_cast<Symbol>(dist(rng)));
            
            drawHeader(credits, totalWins, currentBet, isAllIn);
            std::cout << "\n       ";
            for(auto s : temp) std::cout << CYAN << getDetails(s).icon << " " << RESET;
            std::cout << "\n\n----------------------------------------" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + (i * 10)));
        }
    }

    std::vector<Symbol> getFinalResult() {
        std::vector<Symbol> res;
        std::uniform_int_distribution<int> dist(0, 4);
        for(int i=0; i<3; i++) res.push_back(static_cast<Symbol>(dist(rng)));
        return res;
    }

    int evaluate(const std::vector<Symbol>& res, int bet) {
        if (res[0] == res[1] && res[1] == res[2]) return bet * getDetails(res[0]).multiplier;
        return 0;
    }

    void printVoucher(int totalWins, int finalBalance) {
        std::cout << CLEAR << "\n\n";
        std::cout << "  ************************************" << std::endl;
        std::cout << "  * CASHOUT VOUCHER          *" << std::endl;
        std::cout << "  * ARISTOCRAT TECHNOLOGIES     *" << std::endl;
        std::cout << "  * ENGINEER: SHIVANK SHARMA     *" << std::endl; // NAME ADDED
        std::cout << "  ************************************" << std::endl;
        std::cout << "  * JACKPOT WINS:    $" << totalWins << std::endl;
        std::cout << "  * TOTAL PAYOUT:    $" << finalBalance << std::endl;
        std::cout << "  ************************************" << std::endl;
    }
};

int main() {
    auto engine = std::make_unique<SlotEngine>();
    int credits = 0, totalWins = 0, currentBet = 10;
    bool isAllIn = false;
    std::string input;

    std::cout << "Enter Buy-In Amount: $";
    if (!(std::cin >> credits)) return 0;
    std::getline(std::cin, input); // Flush buffer

    bool running = true;
    while (credits >= 10 && running) {
        engine->drawHeader(credits, totalWins, (isAllIn ? credits : currentBet), isAllIn);
        std::cout << "\n[ENTER] Play | [A] All-In | [X] Collect | [1-4] Set Bet: ";
        std::getline(std::cin, input);

        if (input == "x" || input == "X") {
            if (totalWins > 0) {
                engine->printVoucher(totalWins, credits);
                running = false;
            } else {
                std::cout << RED << "FORFEIT BALANCE? (y/n): " << RESET;
                std::getline(std::cin, input);
                if (input == "y" || input == "Y") running = false;
            }
            continue;
        }

        if (input == "a" || input == "A") { isAllIn = true; continue; }
        if (input == "1") { currentBet = 50; isAllIn = false; continue; }
        if (input == "2") { currentBet = 100; isAllIn = false; continue; }
        if (input == "3") { currentBet = 500; isAllIn = false; continue; }
        if (input == "4") { currentBet = 1000; isAllIn = false; continue; }

        // Logic check for wager
        int wager = isAllIn ? credits : currentBet;
        if (credits < wager) {
            std::cout << RED << "INSUFFICIENT FUNDS!" << RESET << std::endl;
            isAllIn = false; currentBet = 10; // Reset to default
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        credits -= wager;
        engine->spinAnimation(credits, totalWins, wager, isAllIn);
        
        auto res = engine->getFinalResult();
        int win = engine->evaluate(res, wager);
        credits += win;
        totalWins += win;

        if (credits < 10) {
            engine->drawHeader(credits, totalWins, wager, isAllIn);
            std::cout << RED << "\nSYSTEM TILT: BANKROLL DEPLETED.\n" << RESET;
            running = false;
        }
    }

    std::cout << "\nSESSION TERMINATED. GOOD LUCK!\n";
    return 0;
}
