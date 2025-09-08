#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

int main() {
    using clock = std::chrono::steady_clock;
    const int size = 4; // 4x4 board
    const int pairs = (size*size)/2;
    std::vector<int> board;
    for (int i=0;i<pairs;i++){ board.push_back(i); board.push_back(i); }
    std::mt19937 rng(std::random_device{}());
    std::shuffle(board.begin(), board.end(), rng);

    std::vector<bool> revealed(board.size(), false);
    int moves = 0;
    int matches = 0;
    auto start = clock::now();

    while (matches < pairs) {
        // display board
        std::cout << "\nBoard (choose two indices 1-" << board.size() << "):\n";
        for (int i=0;i<(int)board.size();++i) {
            if (i % size == 0) std::cout << "\n";
            if (revealed[i]) std::cout << "[" << board[i] << "] ";
            else std::cout << "[" << (i+1) << "] ";
        }
        std::cout << "\n\nSelect first: ";
        int a,b;
        if (!(std::cin >> a)) return 0;
        std::cout << "Select second: ";
        if (!(std::cin >> b)) return 0;
        if (a<1 || a>(int)board.size() || b<1 || b>(int)board.size() || a==b) {
            std::cout << "Invalid selection.\n";
            continue;
        }
        --a; --b;
        ++moves;
        std::cout << "You revealed: " << board[a] << " and " << board[b] << "\n";
        if (board[a] == board[b]) {
            if (!revealed[a]) { revealed[a]=true; ++matches; }
            if (!revealed[b]) { revealed[b]=true; ++matches; }
            std::cout << "Match!\n";
        } else {
            std::cout << "No match.\n";
        }
    }

    auto dur = std::chrono::duration_cast<std::chrono::seconds>(clock::now() - start).count();
    std::cout << "\nCongratulations — all pairs found!\n";
    std::cout << "Moves: " << moves << "  Time: " << dur << "s\n";
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}