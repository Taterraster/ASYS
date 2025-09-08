#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <limits>

int main() {
    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
    bool play = true;

    while (play) {
        std::cout << "Guess the Number\n";
        std::cout << "Choose difficulty: 1) Easy (1-10)  2) Medium (1-100)  3) Hard (1-1000) : ";
        int diff = 2;
        if (!(std::cin >> diff)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            diff = 2;
        }

        int lo = 1, hi = 100, maxAttempts = 7;
        if (diff == 1) { hi = 10; maxAttempts = 5; }
        else if (diff == 3) { hi = 1000; maxAttempts = 10; }

        std::uniform_int_distribution<int> dist(lo, hi);
        int target = dist(rng);
        int attempts = 0;
        bool won = false;

        std::cout << "I picked a number between " << lo << " and " << hi << ". You have " << maxAttempts << " attempts.\n";
        while (attempts < maxAttempts) {
            std::cout << "Enter guess (" << (attempts+1) << "/" << maxAttempts << "): ";
            int g;
            if (!(std::cin >> g)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Please enter a number.\n";
                continue;
            }
            ++attempts;
            if (g == target) { won = true; break; }
            if (g < target) std::cout << "Too low.\n"; else std::cout << "Too high.\n";
        }

        if (won) std::cout << "Correct! You guessed it in " << attempts << " attempts.\n";
        else std::cout << "Out of attempts. The number was " << target << ".\n";

        std::cout << "Play again? (y/n): ";
        char ch;
        std::cin >> ch;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (ch != 'y' && ch != 'Y') play = false;
    }

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}