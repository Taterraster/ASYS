#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>    // Windows: _kbhit, _getch
#include <cstdlib>
#include <limits>

using namespace std;
using namespace std::chrono;

const int WIDTH = 64;
const int HEIGHT = 20;
const int PADDLE_H = 4;
const int FRAME_MS = 50;

struct Paddle { int y; int x; };
struct Ball { double x, y; double vx, vy; };

static void clearScreen() { std::system("cls"); }

static void draw(const Paddle &L, const Paddle &R, const Ball &b, int scoreL, int scoreR) {
    vector<string> buf(HEIGHT, string(WIDTH, ' '));
    // borders
    for (int x = 0; x < WIDTH; ++x) { buf[0][x] = '-'; buf[HEIGHT-1][x] = '-'; }
    // paddles
    for (int i = 0; i < PADDLE_H; ++i) {
        int ly = L.y + i; if (ly>0 && ly<HEIGHT-1) buf[ly][L.x] = '|';
        int ry = R.y + i; if (ry>0 && ry<HEIGHT-1) buf[ry][R.x] = '|';
    }
    // ball
    int bx = int(b.x + 0.5), by = int(b.y + 0.5);
    if (by>0 && by<HEIGHT-1 && bx>0 && bx<WIDTH-1) buf[by][bx] = 'O';
    // center line (optional)
    for (int y=1; y<HEIGHT-1; ++y) if (y % 2 == 0) buf[y][WIDTH/2] = ':';
    // print
    clearScreen();
    cout << "PONG  Left: W/S   Right: Up/Down   Q to quit\n";
    cout << "Score: " << scoreL << "  -  " << scoreR << "\n\n";
    for (auto &row : buf) cout << row << '\n';
}

int main() {
    cout << "Pong: 1) 2-player  2) vs AI\nChoose mode: ";
    int mode = 1;
    if (!(cin >> mode)) mode = 1;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Paddle L{ HEIGHT/2 - PADDLE_H/2, 2 };
    Paddle R{ HEIGHT/2 - PADDLE_H/2, WIDTH-3 };
    Ball b{ WIDTH/2.0, HEIGHT/2.0, 0.9, 0.2 };
    int scoreL = 0, scoreR = 0;

    auto resetBall = [&](int dir=1){
        b.x = WIDTH/2.0; b.y = HEIGHT/2.0;
        b.vx = 0.9 * dir;
        b.vy = ( (rand()%200) - 100 ) / 200.0;
    };

    resetBall((rand()%2)?1:-1);

    bool running = true;
    while (running) {
        steady_clock::time_point frameStart = steady_clock::now();

        // input (non-blocking)
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0) { // special key (arrows)
                ch = _getch();
                if (ch == 72) { // up
                    if (R.y > 1) R.y--;
                } else if (ch == 80) { // down
                    if (R.y + PADDLE_H < HEIGHT-1) R.y++;
                }
            } else {
                ch = tolower(ch);
                if (ch == 'w') { if (L.y > 1) L.y--; }
                else if (ch == 's') { if (L.y + PADDLE_H < HEIGHT-1) L.y++; }
                else if (ch == 'q') { running = false; break; }
                // handle numeric arrows fallback for some consoles
                else if (ch == 'k') { if (R.y > 1) R.y--; } // alternative up
                else if (ch == 'm') { if (R.y + PADDLE_H < HEIGHT-1) R.y++; } // alternative down
            }
        }

        // AI for right paddle if selected
        if (mode != 1) {
            // simple AI: follow ball with some randomness/lag
            if (b.y < R.y + PADDLE_H/2 && R.y > 1) R.y--;
            else if (b.y > R.y + PADDLE_H/2 && R.y + PADDLE_H < HEIGHT-1) R.y++;
        }

        // update ball
        b.x += b.vx;
        b.y += b.vy;

        // collisions top/bottom
        if (b.y <= 1) { b.y = 1; b.vy = -b.vy; }
        if (b.y >= HEIGHT-2) { b.y = HEIGHT-2; b.vy = -b.vy; }

        int ibx = int(b.x + 0.5);

        // paddle collisions
        if (ibx <= L.x + 1) {
            if (b.y >= L.y && b.y <= L.y + PADDLE_H - 1) {
                b.x = L.x + 2;
                b.vx = -b.vx * 1.05; // speed up
                // tweak vy based on hit position
                double hit = (b.y - L.y) - (PADDLE_H/2.0);
                b.vy += hit * 0.15;
            }
        }
        if (ibx >= R.x - 1) {
            if (b.y >= R.y && b.y <= R.y + PADDLE_H - 1) {
                b.x = R.x - 2;
                b.vx = -b.vx * 1.05;
                double hit = (b.y - R.y) - (PADDLE_H/2.0);
                b.vy += hit * 0.15;
            }
        }

        // scoring
        if (b.x < 0) { scoreR++; resetBall(1); }
        else if (b.x > WIDTH) { scoreL++; resetBall(-1); }

        draw(L,R,b,scoreL,scoreR);

        // frame limiter
        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - frameStart).count();
        if (elapsed < FRAME_MS) std::this_thread::sleep_for(milliseconds(FRAME_MS - elapsed));
    }

    cout << "\nFinal score: " << scoreL << " - " << scoreR << "\n";
    cout << "Press Enter to exit...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}