#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <limits>

using namespace std;

struct Ship { string name; int size; int hits=0; };
struct Cell { bool hasShip=false; bool revealed=false; int shipIndex=-1; };

struct Board {
    int w=10, h=10;
    vector<vector<Cell>> g;
    vector<Ship> ships;
    Board(): g(h, vector<Cell>(w)) {}
    void reset() {
        g.assign(h, vector<Cell>(w));
        ships.clear();
    }
    bool placeShipRandom(int idx, int size, std::mt19937 &rng) {
        uniform_int_distribution<int> dirDist(0,1); // 0 horiz, 1 vert
        uniform_int_distribution<int> xDist(0,w-1), yDist(0,h-1);
        for (int attempt=0; attempt<200; ++attempt) {
            int dir = dirDist(rng);
            int x = xDist(rng), y = yDist(rng);
            int dx = dir==0 ? 1 : 0;
            int dy = dir==1 ? 1 : 0;
            int ex = x + dx*(size-1), ey = y + dy*(size-1);
            if (ex<0 || ex>=w || ey<0 || ey>=h) continue;
            bool ok=true;
            for(int k=0;k<size;++k){
                int nx = x + dx*k, ny = y + dy*k;
                if (g[ny][nx].hasShip) { ok=false; break;}
            }
            if (!ok) continue;
            for(int k=0;k<size;++k){
                int nx = x + dx*k, ny = y + dy*k;
                g[ny][nx].hasShip = true;
                g[ny][nx].shipIndex = idx;
            }
            return true;
        }
        return false;
    }

    bool allSunk() const {
        for (const auto &s : ships) if (s.hits < s.size) return false;
        return true;
    }

    void markShot(int x, int y) {
        g[y][x].revealed = true;
        if (g[y][x].hasShip) {
            int si = g[y][x].shipIndex;
            if (si>=0 && si < (int)ships.size()) ships[si].hits++;
        }
    }
};

static void clearScreen() { std::system("cls"); }

static void printBoards(const Board &player, const Board &opponent, bool showOpponentShips=false) {
    // Print two 10x10 boards side by side: player's own and opponent's (fog of war)
    auto printRowHeader = [](int row){
        cout << (row+1<10 ? " " : "") << (row+1) << " ";
    };
    cout << "    Your Fleet\t\t\t\tOpponent\n\n";
    cout << "   ";
    for (char c='A'; c<'A'+player.w; ++c) cout << c << ' ';
    cout << "\t\t   ";
    for (char c='A'; c<'A'+opponent.w; ++c) cout << c << ' ';
    cout << '\n';
    for (int y=0;y<player.h;++y){
        printRowHeader(y);
        for (int x=0;x<player.w;++x){
            const Cell &c = player.g[y][x];
            char ch = '.';
            if (c.hasShip && c.revealed) ch = 'X';
            else if (c.hasShip) ch = 'S';
            else if (c.revealed) ch = 'o';
            else ch = '.';
            cout << ch << ' ';
        }
        cout << "\t\t";
        printRowHeader(y);
        for (int x=0;x<opponent.w;++x){
            const Cell &c = opponent.g[y][x];
            char ch = '.';
            if (c.revealed) {
                ch = c.hasShip ? 'X' : 'o';
            } else {
                if (showOpponentShips && c.hasShip) ch = 'S';
                else ch = '.';
            }
            cout << ch << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
    // show ship status
    auto printStatus = [](const Board &b, const string &title){
        cout << title << " ships:\n";
        for (const auto &s : b.ships) {
            cout << "  " << s.name << " (" << s.size << ") - " << s.hits << "/" << s.size << " hits\n";
        }
    };
    printStatus(player, "Your");
    cout << '\n';
}

static bool parseCoord(const string &s, int &x, int &y, int w, int h) {
    // Accept formats: A5, a5, 1 5, 5 1, A 5, J10
    string t;
    for (char c : s) if (!isspace((unsigned char)c)) t.push_back(c);
    if (t.empty()) return false;
    if (isalpha((unsigned char)t[0])) {
        char col = t[0];
        col = toupper(col);
        x = col - 'A';
        string rest = t.substr(1);
        if (rest.empty()) return false;
        try {
            y = stoi(rest) - 1;
        } catch(...) { return false; }
    } else {
        // assume "row,col" or "rowcol" where first one or two digits are row
        // try to split digits and letters
        size_t pos = 0;
        while (pos < t.size() && isdigit((unsigned char)t[pos])) pos++;
        if (pos==0) return false;
        string rowStr = t.substr(0,pos);
        string colStr = t.substr(pos);
        if (colStr.empty()) return false;
        y = stoi(rowStr)-1;
        char col = colStr[0];
        if (!isalpha((unsigned char)col)) return false;
        x = toupper(col) - 'A';
    }
    return x>=0 && x<w && y>=0 && y<h;
}

int main() {
    // seed RNG
    std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

    Board player, ai;
    vector<pair<string,int>> shipDefs = {
        {"Carrier",5}, {"Battleship",4}, {"Cruiser",3}, {"Submarine",3}, {"Destroyer",2}
    };

    // Setup ships on both boards
    player.reset(); ai.reset();
    for (auto &sd : shipDefs) { player.ships.push_back({sd.first, sd.second}); ai.ships.push_back({sd.first, sd.second}); }

    cout << "Battleship - console\n";
    cout << "Place ships manually or randomly? (m/r) [r]: ";
    string choice;
    getline(cin, choice);
    if (choice.empty()) choice = "r";
    if (choice[0]=='m' || choice[0]=='M') {
        // manual placement for player
        for (int i=0;i<(int)player.ships.size();++i){
            auto &s = player.ships[i];
            bool placed=false;
            while(!placed){
                clearScreen();
                cout << "Placing your ships. Current board:\n\n";
                printBoards(player, ai, true); // show player ships
                cout << "Place " << s.name << " (size " << s.size << ")\n";
                cout << "Enter start coordinate and direction (e.g. A5 H or A5 V): ";
                string line;
                getline(cin,line);
                if (line.empty()) continue;
                // parse
                int sx, sy;
                if (!parseCoord(line, sx, sy, player.w, player.h)) {
                    cout << "Invalid coordinate. Press Enter to retry."; getline(cin, line); continue;
                }
                // find direction char
                char dir = 'H';
                for (char c : line) if (c=='H' || c=='h' || c=='V' || c=='v') { dir = toupper(c); break; }
                int dx = (dir=='H') ? 1 : 0;
                int dy = (dir=='V') ? 1 : 0;
                int ex = sx + dx*(s.size-1), ey = sy + dy*(s.size-1);
                if (ex < 0 || ex >= player.w || ey < 0 || ey >= player.h) {
                    cout << "Ship doesn't fit. Press Enter to retry."; getline(cin, line); continue;
                }
                bool ok=true;
                for (int k=0;k<s.size;++k){
                    int nx = sx + dx*k, ny = sy + dy*k;
                    if (player.g[ny][nx].hasShip) { ok=false; break; }
                }
                if (!ok) { cout << "Overlaps another ship. Press Enter to retry."; getline(cin, line); continue; }
                for (int k=0;k<s.size;++k){
                    int nx = sx + dx*k, ny = sy + dy*k;
                    player.g[ny][nx].hasShip = true;
                    player.g[ny][nx].shipIndex = i;
                }
                placed = true;
            }
        }
    } else {
        // random placement for player
        for (int i=0;i<(int)player.ships.size();++i){
            int sz = player.ships[i].size;
            bool ok = player.placeShipRandom(i, sz, rng);
            if (!ok) { cout << "Failed to place player ship randomly.\n"; return 1; }
        }
    }

    // random placement for AI
    for (int i=0;i<(int)ai.ships.size();++i){
        int sz = ai.ships[i].size;
        bool ok = ai.placeShipRandom(i, sz, rng);
        if (!ok) { cout << "Failed to place AI ship.\n"; return 1; }
    }

    // AI shooting state
    vector<pair<int,int>> aiCandidates;
    for (int y=0;y<ai.h;++y) for (int x=0;x<ai.w;++x) aiCandidates.emplace_back(x,y);
    shuffle(aiCandidates.begin(), aiCandidates.end(), rng);
    size_t aiIndex = 0;

    // Game loop
    bool playerTurn = true;
    while (true) {
        clearScreen();
        printBoards(player, ai, false);
        if (player.allSunk()) { cout << "All your ships have been sunk. You lose.\n"; break; }
        if (ai.allSunk()) { cout << "You sank all enemy ships. You win!\n"; break; }

        if (playerTurn) {
            cout << "Your turn. Enter target (e.g. A5): ";
            string line; getline(cin,line);
            if (line.empty()) continue;
            int tx, ty;
            if (!parseCoord(line, tx, ty, ai.w, ai.h)) { cout << "Invalid coordinate. Press Enter to continue."; getline(cin,line); continue; }
            if (ai.g[ty][tx].revealed) { cout << "Already shot there. Press Enter."; getline(cin,line); continue; }
            ai.markShot(tx,ty);
            if (ai.g[ty][tx].hasShip) {
                cout << "Hit!\n";
                int si = ai.g[ty][tx].shipIndex;
                if (si>=0 && ai.ships[si].hits == ai.ships[si].size) {
                    cout << "You sank the enemy " << ai.ships[si].name << "!\n";
                }
            } else {
                cout << "Miss.\n";
            }
            cout << "Press Enter to end turn."; getline(cin,line);
            playerTurn = false;
        } else {
            // AI turn: pick next candidate
            if (aiIndex >= aiCandidates.size()) { cout << "AI has no moves left.\n"; break; }
            auto [tx,ty] = aiCandidates[aiIndex++];
            if (player.g[ty][tx].revealed) continue;
            player.markShot(tx,ty);
            cout << "Opponent fires at " << char('A'+tx) << (ty+1) << " : ";
            if (player.g[ty][tx].hasShip) {
                cout << "Hit!\n";
                int si = player.g[ty][tx].shipIndex;
                if (si>=0 && player.ships[si].hits == player.ships[si].size) {
                    cout << "Your " << player.ships[si].name << " was sunk!\n";
                }
            } else cout << "Miss.\n";
            cout << "Press Enter to continue."; string tmp; getline(cin,tmp);
            playerTurn = true;
        }
    }

    cout << "Game over. Press Enter to exit...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}