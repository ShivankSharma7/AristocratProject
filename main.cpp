#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm> 

// --- ANSI COLOR CODES ---
#define RESET   "\033[0m"
#define GOLD    "\033[1;33m"
#define YELLOW  "\033[1;33m" // Added this to fix the error!
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
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
            case Symbol::Cherry:  return {" [ CHERRY ] ", GREEN, 2};
            case Symbol::Lemon:   return {" [ LEMON  ] ", MAGENTA, 5};
            case Symbol::Bell:    return {" [  BELL  ] ", CYAN, 10};
            case Symbol::Seven:   return {" [  SEVEN ] ", RED, 20};
            case Symbol::Jackpot: return {" [ JACKPOT ] ", GOLD, 100};
            default: return {" [  ???   ] ", RESET, 0};
        }
    }

public:
    SlotMachine() {
        rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    }

    void drawFrame(const std::vector<Symbol>& currentReels, int credits, int lastWin) {
        std::cout << CLEAR;
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << GOLD << "       ARISTOCRAT APEX-PRO v2.5         " << RESET << std::endl;
        std::cout << GOLD << "========================================" << RESET << std::endl;
        std::cout << " CREDITS: " << GREEN << "$" << credits << RESET << " | LAST WIN: " << RED << "$" << lastWin << RESET << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "\n       ";
        
        for(auto s : currentReels) {
            SymbolData d = getDetails(s);
            std::cout << d.color << d.icon << RESET;
        }
        
        std::cout << "\n\n----------------------------------------" << std::endl;
        if (lastWin > 0) {
            std::cout << GOLD << "   *** BIG WINNER! CONGRATS! ***" << RESET << std::endl;
        } else {
            std::cout << "      SPIN TO WIN THE JACKPOT!       " << std::endl;
        }
        std::cout << "========================================" << std::endl;
    }

    void spinAnimation(int credits) {
        for (int i = 0; i < 15; ++i) {
            std::vector<Symbol> temp;
            std::uniform_int_distribution<int> dist(0, (int)symbols.size() - 1);
            for(int j=0; j<3; j++) temp.push_back(symbols[dist(rng)]);
            
            drawFrame(temp, credits, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(80 + (i * 10)));
        }
    }

    int evaluate(const std::vector<Symbol>& result, int bet) {
        if (result[0] == result[1] && result[1] == result[2]) {
            return bet * getDetails(result[0]).multiplier;
        }
        return 0;
    }

    std::vector<Symbol> getFinalResult() {
        std::vector<Symbol> res;
        std::uniform_int_distribution<int> dist(0, (int)symbols.size() - 1);
        for(int i=0; i<3; i++) res.push_back(symbols[dist(rng)]);
        return res;
    }
};

int main() {
    SlotMachine game;
    int credits = 0;
    
    std::cout << CLEAR;
    std::cout << RED << "!!! IMPORTANT WARNING !!!" << RESET << std::endl;
    std::cout << "Exiting the game in between spins will result in the " << std::endl;
    std::cout << RED << "LOSS OF ALL REMAINING CREDITS." << RESET << std::endl;
    std::cout << "There is no 'Cash Out' feature enabled in this demo mode." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::cout << "Enter custom starting credits: $";
    if (!(std::cin >> credits)) {
        credits = 100;
    }
    
    std::string input;
    std::getline(std::cin, input); // Flush buffer

    int lastWin = 0;
    bool running = true;

    while (credits >= 10 && running) {
        game.drawFrame({Symbol::Cherry, Symbol::Bell, Symbol::Seven}, credits, lastWin);
        std::cout << "\n[ENTER] to Spin ($10) | [X] to Exit Game: ";
        
        std::getline(std::cin, input);
        
        if (input == "x" || input == "X") {
            std::cout << RED << "\nAre you sure? You will lose all $" << credits << " credits. (y/n): " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm == "y" || confirm == "Y") {
                running = false;
                continue;
            } else {
                continue; 
            }
        }

        credits -= 10;
        game.spinAnimation(credits);
        
        auto finalResult = game.getFinalResult();
        lastWin = game.evaluate(finalResult, 10);
        credits += lastWin;
        
        game.drawFrame(finalResult, credits, lastWin);
        std::cout << "\nSpin complete! Press ENTER to continue...";
        std::getline(std::cin, input); 
    }

    std::cout << CLEAR;
    if (credits < 10 && running) {
        std::cout << RED << "\nGAME OVER: Insufficient Credits." << RESET << std::endl;
    } else {
        std::cout << YELLOW << "\nSESSION TERMINATED: You forfeited your credits." << RESET << std::endl;
    }
    
    std::cout << "Thank you for using the Aristocrat Tech Demo." << std::endl;
    return 0;
}