#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>

static void clearScreen() {
    std::system("cls");
}

struct Connect4 {
    static constexpr int C = 7;
    static constexpr int R = 6;
    std::vector<std::vector<char>> board; // ' ' empty, 'X' player1, 'O' player2

    Connect4() : board(R, std::vector<char>(C, ' ')) {}

    void print() const {
        clearScreen();
        std::cout << "\n Connect 4\n\n";
        for (int r = 0; r < R; ++r) {
            std::cout << "|";
            for (int c = 0; c < C; ++c) {
                std::cout << (board[r][c] == ' ' ? '.' : board[r][c]) << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "+";
        for (int c = 0; c < C; ++c) std::cout << "--";
        std::cout << "+\n ";
        for (int c = 0; c < C; ++c) std::cout << " " << c+1;
        std::cout << "\n\n";
    }

    bool drop(int col, char playerDisc) {
        if (col < 0 || col >= C) return false;
        for (int r = R-1; r >= 0; --r) {
            if (board[r][col] == ' ') {
                board[r][col] = playerDisc;
                return true;
            }
        }
        return false; // column full
    }

    bool full() const {
        for (int c = 0; c < C; ++c) if (board[0][c] == ' ') return false;
        return true;
    }

    bool winner(char disc) const {
        // horizontal
        for (int r = 0; r < R; ++r)
            for (int c = 0; c <= C-4; ++c)
                if (board[r][c]==disc && board[r][c+1]==disc && board[r][c+2]==disc && board[r][c+3]==disc)
                    return true;
        // vertical
        for (int c = 0; c < C; ++c)
            for (int r = 0; r <= R-4; ++r)
                if (board[r][c]==disc && board[r+1][c]==disc && board[r+2][c]==disc && board[r+3][c]==disc)
                    return true;
        // diag down-right
        for (int r = 0; r <= R-4; ++r)
            for (int c = 0; c <= C-4; ++c)
                if (board[r][c]==disc && board[r+1][c+1]==disc && board[r+2][c+2]==disc && board[r+3][c+3]==disc)
                    return true;
        // diag up-right
        for (int r = 3; r < R; ++r)
            for (int c = 0; c <= C-4; ++c)
                if (board[r][c]==disc && board[r-1][c+1]==disc && board[r-2][c+2]==disc && board[r-3][c+3]==disc)
                    return true;
        return false;
    }
};

int main() {
    Connect4 g;
    char cur = 'X';
    bool running = true;
    while (running) {
        g.print();
        std::cout << "Player " << (cur=='X' ? "1 (X)" : "2 (O)") << " - choose column (1-" << Connect4::C << ") or 0 to quit: ";
        int col;
        if (!(std::cin >> col)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (col == 0) break;
        if (!g.drop(col-1, cur)) {
            std::cout << "Invalid move (column full or out of range). Press Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        if (g.winner(cur)) {
            g.print();
            std::cout << "Player " << (cur=='X' ? "1 (X)" : "2 (O)") << " wins!\n";
            running = false;
        } else if (g.full()) {
            g.print();
            std::cout << "It's a tie!\n";
            running = false;
        } else {
            cur = (cur == 'X') ? 'O' : 'X';
        }
    }

    std::cout << "Play again? (y/n): ";
    char resp;
    std::cin >> resp;
    if (resp == 'y' || resp == 'Y') {
        // simple restart by execing the same program: ask user to rebuild/run or just restart loop
        // here restart loop by creating a new game
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        main(); // small and acceptable for quick CLI tool
    }

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}