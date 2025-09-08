// Windows-native console chess vs Stockfish with ASCII board
// Compile (MSVC):   cl /EHsc win_chess.cpp
// Compile (MinGW): g++ -std=c++17 -O2 -o win_chess win_chess.cpp
// Run:             win_chess.exe

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

struct Engine {
    HANDLE hChildStdinWr = nullptr;
    HANDLE hChildStdoutRd = nullptr;
    PROCESS_INFORMATION piProc = {};
};

// ------------------------------------------------------
// Engine communication
// ------------------------------------------------------
bool launch_engine(const std::string &path, Engine &eng) {
    SECURITY_ATTRIBUTES sa{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    HANDLE hStdoutRd = nullptr, hStdoutWr = nullptr;
    HANDLE hStdinRd = nullptr, hStdinWr = nullptr;

    if (!CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0)) return false;
    if (!SetHandleInformation(hStdoutRd, HANDLE_FLAG_INHERIT, 0)) return false;

    if (!CreatePipe(&hStdinRd, &hStdinWr, &sa, 0)) return false;
    if (!SetHandleInformation(hStdinWr, HANDLE_FLAG_INHERIT, 0)) return false;

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.hStdError = hStdoutWr;
    si.hStdOutput = hStdoutWr;
    si.hStdInput = hStdinRd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessA(
        path.c_str(),
        nullptr,
        nullptr, nullptr, TRUE,
        0, nullptr, nullptr,
        &si, &pi
    );
    CloseHandle(hStdoutWr);
    CloseHandle(hStdinRd);

    if (!ok) return false;

    eng.hChildStdoutRd = hStdoutRd;
    eng.hChildStdinWr = hStdinWr;
    eng.piProc = pi;
    return true;
}

void send_cmd(Engine &eng, const std::string &cmd) {
    std::string s = cmd + "\n";
    DWORD written;
    WriteFile(eng.hChildStdinWr, s.c_str(), (DWORD)s.size(), &written, nullptr);
}

std::string read_output(Engine &eng, int timeoutMs = 500) {
    std::string result;
    char buf[4096];
    DWORD avail = 0;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < (DWORD)timeoutMs) {
        if (!PeekNamedPipe(eng.hChildStdoutRd, nullptr, 0, nullptr, &avail, nullptr)) break;
        if (avail == 0) {
            Sleep(50);
            continue;
        }
        DWORD read = 0;
        if (ReadFile(eng.hChildStdoutRd, buf, sizeof(buf)-1, &read, nullptr) && read > 0) {
            buf[read] = 0;
            result += buf;
        }
    }
    return result;
}

// ------------------------------------------------------
// Chessboard utilities
// ------------------------------------------------------
char board[8][8] = {
    {'r','n','b','q','k','b','n','r'},
    {'p','p','p','p','p','p','p','p'},
    {'.','.','.','.','.','.','.','.'},
    {'.','.','.','.','.','.','.','.'},
    {'.','.','.','.','.','.','.','.'},
    {'.','.','.','.','.','.','.','.'},
    {'P','P','P','P','P','P','P','P'},
    {'R','N','B','Q','K','B','N','R'}
};

void print_board() {
    std::cout << "\n";
    for (int r = 0; r < 8; r++) {
        std::cout << (8 - r) << " ";
        for (int c = 0; c < 8; c++) {
            std::cout << board[r][c];
        }
        std::cout << "\n";
    }
    std::cout << "  abcdefgh\n\n";
}

// apply UCI move (like "e2e4" or "e7e8q")
void apply_move(const std::string &mv) {
    if (mv.size() < 4) return;
    int fromCol = mv[0] - 'a';
    int fromRow = 8 - (mv[1] - '0');
    int toCol   = mv[2] - 'a';
    int toRow   = 8 - (mv[3] - '0');

    char piece = board[fromRow][fromCol];
    board[fromRow][fromCol] = '.';

    // promotion
    if (mv.size() == 5) {
        char promo = mv[4];
        if (isupper(piece)) promo = toupper(promo);
        else promo = tolower(promo);
        board[toRow][toCol] = promo;
    } else {
        board[toRow][toCol] = piece;
    }
}

// ------------------------------------------------------
// Main game loop
// ------------------------------------------------------
int main() {
    std::string enginePath = "stockfish\\stockfish-windows-x86-64-avx2.exe";

    Engine eng;
    if (!launch_engine(enginePath, eng)) {
        std::cerr << "Failed to start Stockfish at: " << enginePath << "\n";
        return 1;
    }

    send_cmd(eng, "uci");
    read_output(eng, 1000); // discard engine ID
    send_cmd(eng, "isready");
    read_output(eng, 1000);

    std::vector<std::string> moves;
    bool playerIsWhite = true;

    std::cout << "Choose your side (w = White, b = Black): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore();
    if (choice == 'b' || choice == 'B') playerIsWhite = false;

    std::cout << "You are playing " << (playerIsWhite ? "White" : "Black") << ".\n";
    print_board();

    bool whiteToMove = true;

    // if player chose Black, engine moves first
    if (!playerIsWhite) {
        send_cmd(eng, "position startpos");
        send_cmd(eng, "go movetime 1000");
        std::string out = read_output(eng, 2000);
        size_t p = out.find("bestmove ");
        if (p != std::string::npos) {
            std::string best = out.substr(p + 9);
            best = best.substr(0, best.find(" "));
            std::cout << "Stockfish plays: " << best << "\n";
            moves.push_back(best);
            apply_move(best);
            print_board();
        }
        whiteToMove = false;
    }

    while (true) {
        if ((whiteToMove && playerIsWhite) || (!whiteToMove && !playerIsWhite)) {
            std::cout << "> Your move: ";
            std::string mv;
            std::getline(std::cin, mv);
            if (mv == "quit") break;
            moves.push_back(mv);
            apply_move(mv);
            print_board();
        } else {
            std::ostringstream pos;
            pos << "position startpos";
            if (!moves.empty()) {
                pos << " moves";
                for (auto &m : moves) pos << " " << m;
            }
            send_cmd(eng, pos.str());
            send_cmd(eng, "go movetime 1000");

            std::string out = read_output(eng, 2000);
            size_t p = out.find("bestmove ");
            if (p != std::string::npos) {
                std::string best = out.substr(p + 9);
                best = best.substr(0, best.find(" "));
                if (best == "(none)") {
                    std::cout << "Game over.\n";
                    break;
                }
                std::cout << "Stockfish plays: " << best << "\n";
                moves.push_back(best);
                apply_move(best);
                print_board();
            }
        }
        whiteToMove = !whiteToMove;
    }

    send_cmd(eng, "quit");
    TerminateProcess(eng.piProc.hProcess, 0);
    CloseHandle(eng.piProc.hThread);
    CloseHandle(eng.piProc.hProcess);
    CloseHandle(eng.hChildStdinWr);
    CloseHandle(eng.hChildStdoutRd);
}
