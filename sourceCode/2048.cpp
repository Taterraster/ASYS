#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <conio.h>     // _kbhit, _getch on Windows
#include <iomanip>
#include <algorithm>

using Grid = std::vector<std::vector<int>>;

static void clearScreen() { std::system("cls"); }

static void printGrid(const Grid &g, int score) {
    clearScreen();
    std::cout << "2048 (use arrow keys). R = restart, Q = quit\n";
    std::cout << "Score: " << score << "\n\n";
    for (int r = 0; r < 4; ++r) {
        std::cout << "+------+------+------+------+\n";
        for (int c = 0; c < 4; ++c) {
            int v = g[r][c];
            if (v == 0) std::cout << "|      ";
            else {
                std::ostringstream ss;
                ss << v;
                std::string s = ss.str();
                int pad = 6 - (int)s.size();
                int left = pad/2;
                int right = pad - left;
                std::cout << "|" << std::string(left, ' ') << s << std::string(right, ' ');
            }
        }
        std::cout << "|\n";
    }
    std::cout << "+------+------+------+------+\n";
}

static int spawnTile(Grid &g, std::mt19937 &rng) {
    std::vector<std::pair<int,int>> empties;
    for (int r=0;r<4;++r) for (int c=0;c<4;++c) if (g[r][c]==0) empties.emplace_back(r,c);
    if (empties.empty()) return 0;
    std::uniform_int_distribution<int> d(0,(int)empties.size()-1);
    auto p = empties[d(rng)];
    std::uniform_int_distribution<int> v(0,9); // 10% chance 4
    g[p.first][p.second] = (v(rng) == 0) ? 4 : 2;
    return g[p.first][p.second];
}

static bool canMove(const Grid &g) {
    for (int r=0;r<4;++r) for (int c=0;c<4;++c) {
        if (g[r][c] == 0) return true;
        if (c+1<4 && g[r][c] == g[r][c+1]) return true;
        if (r+1<4 && g[r][c] == g[r+1][c]) return true;
    }
    return false;
}

// slide/merge to left; returns pair<moved, gainedScore>
static std::pair<bool,int> moveLeft(Grid &g) {
    bool moved = false;
    int gained = 0;
    for (int r=0;r<4;++r) {
        // compress
        std::vector<int> row;
        for (int c=0;c<4;++c) if (g[r][c]!=0) row.push_back(g[r][c]);
        int origSize = (int)row.size();
        row.resize(4,0);
        // merge
        for (int i=0;i<3;++i) {
            if (row[i] != 0 && row[i] == row[i+1]) {
                row[i] *= 2;
                gained += row[i];
                row[i+1] = 0;
            }
        }
        // compress again
        std::vector<int> finalRow;
        for (int x : row) if (x!=0) finalRow.push_back(x);
        finalRow.resize(4,0);
        for (int c=0;c<4;++c) {
            if (g[r][c] != finalRow[c]) moved = true;
            g[r][c] = finalRow[c];
        }
    }
    return {moved, gained};
}

// helpers: rotate grid clockwise n times
static void rotate(Grid &g) {
    Grid tmp = g;
    for (int r=0;r<4;++r) for (int c=0;c<4;++c) tmp[c][3-r] = g[r][c];
    g = tmp;
}

int main() {
    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
    auto makeNew = [&](){
        Grid g(4, std::vector<int>(4,0));
        int score = 0;
        spawnTile(g, rng);
        spawnTile(g, rng);
        return std::pair<Grid,int>(g,score);
    };

    auto state = makeNew();
    Grid &grid = state.first;
    int score = state.second;
    bool running = true;

    while (running) {
        printGrid(grid, score);
        if (std::any_of(grid.begin(), grid.end(), [](const std::vector<int>& row){
            return std::any_of(row.begin(), row.end(), [](int v){ return v==2048; });
        })) {
            std::cout << "You reached 2048! Continue playing? (press arrow to continue, R to restart, Q to quit)\n";
        }
        if (!canMove(grid)) {
            std::cout << "No moves left. Game over. Press R to restart or Q to quit.\n";
        }

        int ch = _getch();
        bool moved=false;
        int gained=0;
        if (ch == 0 || ch == 0xE0) {
            int arrow = _getch();
            // arrow: 72 up, 80 down, 75 left, 77 right
            if (arrow == 75) { // left
                auto p = moveLeft(grid); moved = p.first; gained = p.second;
            } else if (arrow == 77) { // right
                // rotate twice, move left, rotate twice
                rotate(grid); rotate(grid);
                auto p = moveLeft(grid); moved = p.first; gained = p.second;
                rotate(grid); rotate(grid);
            } else if (arrow == 72) { // up
                rotate(grid); rotate(grid); rotate(grid); // rotate left once
                auto p = moveLeft(grid); moved = p.first; gained = p.second;
                rotate(grid); // rotate back
            } else if (arrow == 80) { // down
                rotate(grid); // rotate right once
                auto p = moveLeft(grid); moved = p.first; gained = p.second;
                rotate(grid); rotate(grid); rotate(grid);
            }
        } else {
            char c = (char)ch;
            if (c == 'q' || c == 'Q') break;
            if (c == 'r' || c == 'R') {
                state = makeNew();
                grid = state.first;
                score = state.second;
                continue;
            }
        }

        if (moved) {
            score += gained;
            spawnTile(grid, rng);
        }
    }

    std::cout << "Final score: " << score << "\n";
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}