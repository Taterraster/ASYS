#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cctype>
#include <set>
#include <chrono>
#include <limits>

static void printGallows(int wrong) {
    // 7 stages (0..6)
    static const char *stages[] = {
        "  +---+\n  |   |\n      |\n      |\n      |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n=========\n",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n=========\n"
    };
    std::cout << stages[std::min(std::max(wrong,0),6)] << '\n';
}

static std::string pickWord(std::mt19937 &rng) {
    static const std::vector<std::string> words = {
        "programming","computer","hangman","wizard","network","variable",
        "function","compile","keyboard","repository","algorithm","random",
        "challenge","sandbox","developer"
    };
    std::uniform_int_distribution<size_t> d(0, words.size()-1);
    return words[d(rng)];
}

int main() {
    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
    bool play = true;

    while (play) {
        std::string word = pickWord(rng);
        std::string masked(word.size(), '_');
        std::set<char> guessed;
        int wrong = 0;
        const int maxWrong = 6;
        bool won = false;

        while (wrong <= maxWrong && !won) {
            std::cout << "\n";
            printGallows(wrong);
            std::cout << "Word: ";
            for (size_t i = 0; i < word.size(); ++i) {
                if (guessed.count(std::tolower(word[i]))) std::cout << word[i];
                else std::cout << '_';
                if (i+1 < word.size()) std::cout << ' ';
            }
            std::cout << "\nGuessed letters: ";
            for (char c : guessed) std::cout << c << ' ';
            std::cout << "\nWrong: " << wrong << "/" << maxWrong << "\n";

            std::cout << "Enter a letter or guess the whole word: ";
            std::string in;
            if (!std::getline(std::cin, in)) return 0;
            if (in.empty()) { std::cout << "No input.\n"; continue; }

            // trim
            while (!in.empty() && std::isspace((unsigned char)in.back())) in.pop_back();
            size_t pos = 0; while (pos < in.size() && std::isspace((unsigned char)in[pos])) ++pos;
            if (pos) in.erase(0,pos);

            // guess whole word
            if (in.size() > 1) {
                std::string guess = in;
                std::transform(guess.begin(), guess.end(), guess.begin(), [](unsigned char c){ return std::tolower(c); });
                std::string lowerWord = word; std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), [](unsigned char c){ return std::tolower(c); });
                if (guess == lowerWord) {
                    won = true;
                    break;
                } else {
                    ++wrong;
                    std::cout << "Wrong word guess.\n";
                }
                continue;
            }

            // single letter
            char ch = std::tolower(in[0]);
            if (!std::isalpha((unsigned char)ch)) { std::cout << "Please enter a letter.\n"; continue; }
            if (guessed.count(ch)) { std::cout << "Already guessed.\n"; continue; }
            guessed.insert(ch);

            bool hit = false;
            for (char wc : word) {
                if (std::tolower(wc) == ch) { hit = true; break; }
            }
            if (!hit) {
                ++wrong;
                std::cout << "Nope.\n";
            } else {
                std::cout << "Nice!\n";
            }

            // check if all revealed
            bool allRevealed = true;
            for (char wc : word) {
                if (!guessed.count(std::tolower(wc))) { allRevealed = false; break; }
            }
            if (allRevealed) won = true;
        }

        printGallows(wrong);
        if (won) std::cout << "You win! The word was: " << word << "\n";
        else std::cout << "You lose. The word was: " << word << "\n";

        // replay prompt
        std::cout << "Play again? (y/n): ";
        std::string resp;
        if (!std::getline(std::cin, resp)) return 0;
        if (!resp.empty() && (resp[0]=='y' || resp[0]=='Y')) play = true;
        else play = false;
    }
}