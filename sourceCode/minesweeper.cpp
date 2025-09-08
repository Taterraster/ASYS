#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <queue>
#include <chrono>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <algorithm>   // <-- added for std::shuffle
#include <thread>      // <-- added for std::this_thread::sleep_for

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adj = 0;
};

using Board = std::vector<std::vector<Cell>>;

static void clearScreen() {
    std::system("cls");
}

static void printBoard(const Board &b, bool revealAll = false) {
    int h = (int)b.size();
    int w = (int)b[0].size();
    std::cout << "    ";
    for (int x = 0; x < w; ++x) std::cout << std::setw(2) << x << ' ';
    std::cout << "\n   +" << std::string(w * 3, '-') << "+\n";
    for (int y = 0; y < h; ++y) {
        std::cout << std::setw(2) << y << " |";
        for (int x = 0; x < w; ++x) {
            const Cell &c = b[y][x];
            if (revealAll) {
                if (c.mine) std::cout << " * ";
                else if (c.adj > 0) std::cout << ' ' << c.adj << ' ';
                else std::cout << "   ";
            } else {
                if (c.revealed) {
                    if (c.mine) std::cout << " * ";
                    else if (c.adj > 0) std::cout << ' ' << c.adj << ' ';
                    else std::cout << "   ";
                } else if (c.flagged) {
                    std::cout << " F ";
                } else {
                    std::cout << " . ";
                }
            }
        }
        std::cout << "|\n";
    }
    std::cout << "   +" << std::string(w * 3, '-') << "+\n";
}

// neighbor offsets
static const int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
static const int dy[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

static bool inBounds(int x, int y, int w, int h) {
    return x >= 0 && y >= 0 && x < w && y < h;
}

// place mines after first move so first reveal is never a mine
static void placeMines(Board &b, int mines, int safeX, int safeY, std::mt19937 &rng) {
    int h = (int)b.size(), w = (int)b[0].size();
    std::vector<int> idx;
    idx.reserve(w*h);
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) {
        // skip the initial safe cell and its neighbors
        if (std::abs(x - safeX) <= 1 && std::abs(y - safeY) <= 1) continue;
        idx.push_back(y*w + x);
    }
    std::shuffle(idx.begin(), idx.end(), rng);
    for (int i=0;i<mines && i<(int)idx.size();++i) {
        int v = idx[i];
        int x = v % w, y = v / w;
        b[y][x].mine = true;
    }
    // compute adjacency
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) {
        int count = 0;
        for (int k=0;k<8;++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (inBounds(nx, ny, w, h) && b[ny][nx].mine) ++count;
        }
        b[y][x].adj = count;
    }
}

// flood-fill reveal for zero-adj cells
static void floodReveal(Board &b, int sx, int sy) {
    int h = (int)b.size(), w = (int)b[0].size();
    std::queue<std::pair<int,int>> q;
    q.push({sx,sy});
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        for (int k=0;k<8;++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (!inBounds(nx, ny, w, h)) continue;
            Cell &c = b[ny][nx];
            if (c.revealed || c.flagged) continue;
            c.revealed = true;
            if (c.adj == 0 && !c.mine) q.push({nx,ny});
        }
    }
}

static bool checkWin(const Board &b, int mines) {
    int h = (int)b.size(), w = (int)b[0].size();
    int revealed = 0;
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) if (b[y][x].revealed) ++revealed;
    return (revealed + mines) == w*h;
}

int main() {
    int w = 9, h = 9, mines = 10;
    std::cout << "Minesweeper - console\n";
    std::cout << "Enter width height mines (or press Enter for default 9 9 10): ";
    std::string line;
    std::getline(std::cin, line);
    if (!line.empty()) {
        std::istringstream iss(line);
        iss >> w >> h >> mines;
        if (w < 5) w = 5; if (h < 5) h = 5;
        if (mines < 1) mines = 1;
        if (mines > w*h - 1) mines = w*h - 1;
    }

    Board board(h, std::vector<Cell>(w));
    bool firstMove = true;
    bool lost = false;
    bool won = false;

    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

    while (!lost && !won) {
        clearScreen();
        printBoard(board);
        std::cout << "Commands:\n";
        std::cout << " r X Y    reveal at (X Y)\n";
        std::cout << " f X Y    toggle flag at (X Y)\n";
        std::cout << " q        quit\n";
        std::cout << "Enter command: ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd; iss >> cmd;
        if (cmd == "q" || cmd == "Q") break;
        if (cmd == "r" || cmd == "R") {
            int x,y; if (!(iss >> x >> y)) { std::cout << "Invalid coords\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }
            if (!inBounds(x,y,w,h)) { std::cout << "Out of bounds\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }
            if (board[y][x].flagged) { std::cout << "Flagged - unflag to reveal\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }

            if (firstMove) {
                placeMines(board, mines, x, y, rng);
                firstMove = false;
            }
            Cell &c = board[y][x];
            c.revealed = true;
            if (c.mine) {
                lost = true;
                break;
            }
            if (c.adj == 0) floodReveal(board, x, y);
            if (checkWin(board, mines)) { won = true; break; }
        } else if (cmd == "f" || cmd == "F") {
            int x,y; if (!(iss >> x >> y)) { std::cout << "Invalid coords\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }
            if (!inBounds(x,y,w,h)) { std::cout << "Out of bounds\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }
            if (board[y][x].revealed) { std::cout << "Already revealed\n"; std::this_thread::sleep_for(std::chrono::milliseconds(600)); continue; }
            board[y][x].flagged = !board[y][x].flagged;
        } else {
            std::cout << "Unknown command\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
        }
    }

    clearScreen();
    printBoard(board, true);
    if (won) std::cout << "You win! Congratulations.\n";
    else if (lost) std::cout << "Boom! You hit a mine.\n";
    else std::cout << "Exited.\n";

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}