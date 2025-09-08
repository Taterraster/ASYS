#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h>    // _kbhit, _getch on Windows
#include <cstdlib>
#include <limits>

using coord = std::pair<int,int>;

enum Direction { UP, DOWN, LEFT, RIGHT };

static void clearScreen() {
    // simple clear for Windows console
    std::system("cls");
}

static void draw(const std::vector<std::string>& grid, int score) {
    clearScreen();
    for (const auto &row : grid) std::cout << row << '\n';
    std::cout << "Score: " << score << "\n";
    std::cout << "Controls: WASD or arrow keys. Press Q to quit.\n";
}

static Direction readInput(Direction current) {
    if (!_kbhit()) return current;
    int ch = _getch();
    if (ch == 0 || ch == 0xE0) { // arrow keys
        ch = _getch();
        switch (ch) {
            case 72: return UP;    // up
            case 80: return DOWN;  // down
            case 75: return LEFT;  // left
            case 77: return RIGHT; // right
            default: return current;
        }
    } else {
        ch = std::tolower(ch);
        if (ch == 'w') return UP;
        if (ch == 's') return DOWN;
        if (ch == 'a') return LEFT;
        if (ch == 'd') return RIGHT;
        if (ch == 'q') { std::exit(0); } // quit instantly
    }
    return current;
}

int main() {
    const int width = 32;
    const int height = 20;
    const int frameMs = 100; // lower = faster

    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dx(1, width-2), dy(1, height-2);

    std::deque<coord> snake;
    snake.push_back({width/2, height/2});
    snake.push_back({width/2-1, height/2});
    snake.push_back({width/2-2, height/2});
    Direction dir = RIGHT;

    coord food{dx(rng), dy(rng)};
    int score = 0;
    bool gameOver = false;

    while (!gameOver) {
        // input (non-blocking)
        Direction newDir = readInput(dir);
        // prevent immediate reverse
        if (!((dir==LEFT && newDir==RIGHT) || (dir==RIGHT && newDir==LEFT) ||
              (dir==UP && newDir==DOWN) || (dir==DOWN && newDir==UP))) {
            dir = newDir;
        }

        // move head
        coord head = snake.front();
        coord next = head;
        switch(dir) {
            case UP:    next.second -= 1; break;
            case DOWN:  next.second += 1; break;
            case LEFT:  next.first -= 1; break;
            case RIGHT: next.first += 1; break;
        }

        // check collisions with walls
        if (next.first <= 0 || next.first >= width-1 || next.second <= 0 || next.second >= height-1) {
            gameOver = true;
        } else {
            // check self-collision
            for (const auto &p : snake) if (p == next) { gameOver = true; break; }
        }

        if (gameOver) break;

        // move snake
        snake.push_front(next);
        bool ate = (next == food);
        if (ate) {
            ++score;
            // spawn new food not on snake
            while (true) {
                coord cand{dx(rng), dy(rng)};
                bool onSnake = false;
                for (const auto &p : snake) if (p == cand) { onSnake = true; break; }
                if (!onSnake) { food = cand; break; }
            }
        } else {
            snake.pop_back();
        }

        // build grid
        std::vector<std::string> grid(height, std::string(width, ' '));
        // borders
        for (int x=0;x<width;++x) { grid[0][x] = '#'; grid[height-1][x] = '#'; }
        for (int y=0;y<height;++y) { grid[y][0] = '#'; grid[y][width-1] = '#'; }

        // food
        grid[food.second][food.first] = '@';

        // snake
        bool headDrawn = false;
        for (size_t i=0;i<snake.size();++i) {
            auto [sx, sy] = snake[i];
            if (i==0) { grid[sy][sx] = 'O'; headDrawn = true; }
            else grid[sy][sx] = 'o';
        }

        draw(grid, score);

        // frame delay
        std::this_thread::sleep_for(std::chrono::milliseconds(frameMs));
    }

    // game over
    std::cout << "\nGame Over. Final score: " << score << "\n";
    std::cout << "Press Enter to return...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}