#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <random>
#include <chrono>
#include <limits>
#include <cstdlib>
#include <cmath>

using Board = std::vector<std::string>; // 8x8, '.' empty

struct Game {
    Board b;
    bool whiteToMove = true;
    bool wkMoved = false, wqMoved = false;
    bool bkMoved = false, bqMoved = false;
    std::pair<int,int> enPassant = {-1,-1};
    int fullmoves = 1;

    struct Snap {
        Board b;
        bool whiteToMove;
        bool wkMoved, wqMoved, bkMoved, bqMoved;
        std::pair<int,int> enPassant;
        int fullmoves;
    };
    std::vector<Snap> history;

    Snap snapshot() const {
        return Snap{b, whiteToMove, wkMoved, wqMoved, bkMoved, bqMoved, enPassant, fullmoves};
    }
    void restore(const Snap &s) {
        b = s.b;
        whiteToMove = s.whiteToMove;
        wkMoved = s.wkMoved; wqMoved = s.wqMoved; bkMoved = s.bkMoved; bqMoved = s.bqMoved;
        enPassant = s.enPassant;
        fullmoves = s.fullmoves;
    }
    bool canUndo() const { return !history.empty(); }
    bool undo() {
        if (!canUndo()) return false;
        restore(history.back());
        history.pop_back();
        return true;
    }
    void clearHistory() { history.clear(); }

    Game() { reset(); }

    void reset() {
        b = {
            "rnbqkbnr",
            "pppppppp",
            "........",
            "........",
            "........",
            "........",
            "PPPPPPPP",
            "RNBQKBNR"
        };
        whiteToMove = true;
        wkMoved = wqMoved = bkMoved = bqMoved = false;
        enPassant = {-1,-1};
        fullmoves = 1;
        clearHistory();
    }

    static bool inBounds(int r,int c){ return r>=0 && r<8 && c>=0 && c<8; }
    static bool isWhite(char p){ return std::isupper((unsigned char)p); }
    static bool isBlack(char p){ return std::islower((unsigned char)p); }
    static bool empty(char p){ return p=='.'; }

    void print() const {
        std::system("cls");
        std::cout << "   a b c d e f g h\n";
        for (int r=0;r<8;++r){
            std::cout << 8-r << "  ";
            for (int c=0;c<8;++c){
                char ch = b[r][c];
                std::cout << (ch=='.'?'.':ch) << ' ';
            }
            std::cout << " " << 8-r << "\n";
        }
        std::cout << "   a b c d e f g h\n";
        std::cout << (whiteToMove ? "White" : "Black") << " to move"
                  << "  (enter moves like e2e4 or e7e8q for promotion)\n";
    }

    static bool algToRC(const std::string &s, int &r, int &c){
        if (s.size()<2) return false;
        char f = std::tolower(s[0]);
        char R = s[1];
        if (f<'a' || f>'h' || R<'1' || R>'8') return false;
        c = f - 'a';
        r = 8 - (R - '0');
        return true;
    }

    bool pathClear(int sr,int sc,int tr,int tc) const {
        int dr = (tr>sr) ? 1 : (tr<sr) ? -1 : 0;
        int dc = (tc>sc) ? 1 : (tc<sc) ? -1 : 0;
        int r = sr + dr, c = sc + dc;
        while (r != tr || c != tc) {
            if (!inBounds(r,c)) return false;
            if (!empty(b[r][c])) return false;
            r += dr; c += dc;
        }
        return true;
    }

    bool isAttacked(int tr,int tc,bool attackerIsWhite) const {
        for (int sr=0; sr<8; ++sr) for (int sc=0; sc<8; ++sc) {
            char p = b[sr][sc];
            if (p=='.') continue;
            if (attackerIsWhite != isWhite(p)) continue;
            char lower = std::tolower((unsigned char)p);
            int dr = tr - sr, dc = tc - sc;
            int adr = std::abs(dr), adc = std::abs(dc);
            switch (lower) {
                case 'p': {
                    int dir = attackerIsWhite ? -1 : 1;
                    if (dr == dir && std::abs(dc) == 1) return true;
                    break;
                }
                case 'n': if ((adr==2 && adc==1) || (adr==1 && adc==2)) return true; break;
                case 'b': if (adr==adc && adr>0 && pathClear(sr,sc,tr,tc)) return true; break;
                case 'r': if ((adr==0 && adc>0 || adr>0 && adc==0) && pathClear(sr,sc,tr,tc)) return true; break;
                case 'q': if (((adr==adc && adr>0) || (adr==0 && adc>0) || (adr>0 && adc==0)) && pathClear(sr,sc,tr,tc)) return true; break;
                case 'k': if (std::max(adr,adc)==1) return true; break;
                default: break;
            }
        }
        return false;
    }

    bool findKing(bool white, int &kr,int &kc) const {
        char K = white ? 'K' : 'k';
        for (int r=0;r<8;++r) for (int c=0;c<8;++c) if (b[r][c]==K) { kr=r; kc=c; return true; }
        return false;
    }

    bool pseudoLegal(int sr,int sc,int tr,int tc, char /*promo*/ = 0) const {
        if (!inBounds(sr,sc) || !inBounds(tr,tc)) return false;
        char p = b[sr][sc];
        if (p=='.') return false;
        bool movingWhite = isWhite(p);
        if (movingWhite != whiteToMove) return false;
        char target = b[tr][tc];
        if (target != '.' && (isWhite(target) == movingWhite)) return false;
        int dr = tr - sr, dc = tc - sc;
        int adr = std::abs(dr), adc = std::abs(dc);
        char lower = std::tolower((unsigned char)p);
        if (lower == 'p') {
            int dir = movingWhite ? -1 : 1;
            if (dc==0 && dr==dir && target=='.') return true;
            if (dc==0 && dr==2*dir && target=='.') {
                int startRow = movingWhite ? 6 : 1;
                if (sr==startRow && b[sr+dir][sc]=='.') return true;
                return false;
            }
            if (adr==1 && adc==1) {
                if (target!='.' && (isWhite(target) != movingWhite)) return true;
                if (target=='.' && enPassant.first==tr && enPassant.second==tc) return true;
            }
            return false;
        } else if (lower == 'n') {
            return (adr==2 && adc==1) || (adr==1 && adc==2);
        } else if (lower == 'b') {
            return (adr==adc && adr>0 && pathClear(sr,sc,tr,tc));
        } else if (lower == 'r') {
            return ((adr==0 && adc>0) || (adr>0 && adc==0)) && pathClear(sr,sc,tr,tc);
        } else if (lower == 'q') {
            return (((adr==adc && adr>0) || (adr==0 && adc>0) || (adr>0 && adc==0)) && pathClear(sr,sc,tr,tc));
        } else if (lower == 'k') {
            if (std::max(adr,adc) == 1) return true;
            if (adr==0 && adc==2) {
                if (movingWhite) {
                    if (wkMoved) return false;
                    if (tc > sc) {
                        if (b[7][7] != 'R') return false;
                        if (b[7][5] != '.' || b[7][6] != '.') return false;
                        return true;
                    } else {
                        if (b[7][0] != 'R') return false;
                        if (b[7][1] != '.' || b[7][2] != '.' || b[7][3] != '.') return false;
                        return true;
                    }
                } else {
                    if (bkMoved) return false;
                    if (tc > sc) {
                        if (b[0][7] != 'r') return false;
                        if (b[0][5] != '.' || b[0][6] != '.') return false;
                        return true;
                    } else {
                        if (b[0][0] != 'r') return false;
                        if (b[0][1] != '.' || b[0][2] != '.' || b[0][3] != '.') return false;
                        return true;
                    }
                }
            }
            return false;
        }
        return false;
    }

    bool makeMoveIfLegal(int sr,int sc,int tr,int tc, char promo = 0) {
        if (!pseudoLegal(sr,sc,tr,tc,promo)) return false;
        Game copy = *this;
        char moving = copy.b[sr][sc];
        char lower = std::tolower((unsigned char)moving);
        bool movingWhite = isWhite(moving);
        copy.enPassant = {-1,-1};

        if (lower=='k' && sr== (movingWhite?7:0) && std::abs(tc-sc)==2) {
            copy.b[sr][sc] = '.';
            copy.b[tr][tc] = moving;
            if (movingWhite) {
                if (tc>sc) { copy.b[7][7] = '.'; copy.b[7][5] = 'R'; }
                else { copy.b[7][0] = '.'; copy.b[7][3] = 'R'; }
                copy.wkMoved = true;
            } else {
                if (tc>sc) { copy.b[0][7] = '.'; copy.b[0][5] = 'r'; }
                else { copy.b[0][0] = '.'; copy.b[0][3] = 'r'; }
                copy.bkMoved = true;
            }
        } else {
            if (lower=='p' && sc!=tc && copy.b[tr][tc]=='.' && copy.enPassant.first==tr && copy.enPassant.second==tc) {
                int capr = movingWhite ? tr+1 : tr-1;
                copy.b[capr][tc] = '.';
            }
            copy.b[tr][tc] = copy.b[sr][sc];
            copy.b[sr][sc] = '.';
            if (lower=='p' && std::abs(tr-sr)==2) {
                int epR = (sr + tr)/2;
                copy.enPassant = {epR, sc};
            }
            if (lower=='p') {
                if (movingWhite && tr==0) {
                    char p = promo ? std::toupper(promo) : 'Q';
                    copy.b[tr][tc] = p;
                } else if (!movingWhite && tr==7) {
                    char p = promo ? std::tolower(promo) : 'q';
                    copy.b[tr][tc] = p;
                }
            }
            if (lower=='k') {
                if (movingWhite) copy.wkMoved = true; else copy.bkMoved = true;
            }
            if (lower=='r') {
                if (movingWhite) {
                    if (sr==7 && sc==7) copy.wkMoved = true;
                    if (sr==7 && sc==0) copy.wqMoved = true;
                } else {
                    if (sr==0 && sc==7) copy.bkMoved = true;
                    if (sr==0 && sc==0) copy.bqMoved = true;
                }
            }
        }

        int kr=-1,kc=-1;
        if (!copy.findKing(movingWhite, kr, kc)) return false;
        if (copy.isAttacked(kr,kc, !movingWhite)) return false;

        history.push_back(snapshot());
        if (history.size() > 2000) history.erase(history.begin());
        *this = copy;
        whiteToMove = !whiteToMove;
        if (!whiteToMove) ++fullmoves;
        return true;
    }

    struct Move { int sr,sc,tr,tc; char promo; };
    std::vector<Move> legalMoves() const {
        std::vector<Move> out;
        for (int sr=0; sr<8; ++sr) for (int sc=0; sc<8; ++sc) {
            char p = b[sr][sc];
            if (p=='.') continue;
            if (isWhite(p) != whiteToMove) continue;
            for (int tr=0; tr<8; ++tr) for (int tc=0; tc<8; ++tc) {
                if (!pseudoLegal(sr,sc,tr,tc)) continue;
                if (std::tolower((unsigned char)p)=='p' && (tr==0 || tr==7)) {
                    const char promos[] = {'q','r','b','n'};
                    for (char pr : promos) {
                        Game copy = *this;
                        if (copy.makeMoveIfLegal(sr,sc,tr,tc,pr)) out.push_back({sr,sc,tr,tc,pr});
                    }
                } else {
                    Game copy = *this;
                    if (copy.makeMoveIfLegal(sr,sc,tr,tc,0)) out.push_back({sr,sc,tr,tc,0});
                }
            }
        }
        return out;
    }
};

static bool parseMoveStr(const std::string &in, int &sr,int &sc,int &tr,int &tc,char &promo) {
    std::string s;
    for (char ch : in) if (!std::isspace((unsigned char)ch)) s.push_back(ch);
    if (s.size() < 4) return false;
    std::string a = s.substr(0,2), b = s.substr(2,2);
    if (!Game::algToRC(a,sr,sc)) return false;
    if (!Game::algToRC(b,tr,tc)) return false;
    promo = 0;
    if (s.size()>=5) promo = s[4];
    return true;
}

static Game::Move pickRandom(const std::vector<Game::Move> &moves) {
    static std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<size_t> d(0, moves.size()-1);
    return moves[d(rng)];
}

int main(){
    Game g;
    bool autoPlayBlack = false;
    std::cout << "Console Chess (castling, en-passant, promotion, undo). Type 'help'.\n";
    std::cout << "Play vs random AI? (y/N): ";
    std::string yn; std::getline(std::cin, yn);
    if (!yn.empty() && (yn[0]=='y' || yn[0]=='Y')) autoPlayBlack = true;

    while (true) {
        g.print();
        int kr,kc;
        g.findKing(g.whiteToMove, kr, kc);
        bool inCheck = g.isAttacked(kr,kc, !g.whiteToMove);
        auto moves = g.legalMoves();
        if (moves.empty()) {
            if (inCheck) std::cout << (g.whiteToMove ? "Checkmate. Black wins.\n" : "Checkmate. White wins.\n");
            else std::cout << "Stalemate.\n";
            break;
        }
        if (inCheck) std::cout << "Your king is in check!\n";

        if (!g.whiteToMove && autoPlayBlack) {
            auto mv = pickRandom(moves);
            char srcf = 'a' + mv.sc, dstf = 'a' + mv.tc;
            int srank = 8 - mv.sr, trank = 8 - mv.tr;
            std::cout << "Black plays " << srcf << srank << dstf << trank;
            if (mv.promo) std::cout << mv.promo;
            std::cout << "\nPress Enter to continue..."; std::string tmp; std::getline(std::cin,tmp);
            g.makeMoveIfLegal(mv.sr,mv.sc,mv.tr,mv.tc,mv.promo);
            continue;
        }

        std::cout << "Enter move (e2e4), 'moves', 'undo', 'reset', 'resign' or 'quit': ";
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) continue;
        if (line=="quit" || line=="resign") { std::cout << "Game ended.\n"; break; }
        if (line=="help") {
            std::cout << "Commands: move (e2e4), moves, undo, reset, resign/quit, help\n";
            std::cout << "Promotion: append q/r/b/n to move, e.g. e7e8q\n";
            std::cout << "Castling: e1g1 or e1c1 (white), e8g8 or e8c8 (black)\n";
            std::cout << "En-passant supported automatically\n";
            std::cout << "Press Enter to continue..."; std::getline(std::cin,line); continue;
        }
        if (line=="moves") {
            for (auto &m : moves) {
                char sf = 'a' + m.sc, df = 'a' + m.tc;
                int sr = 8 - m.sr, tr = 8 - m.tr;
                std::cout << sf << sr << df << tr;
                if (m.promo) std::cout << m.promo;
                std::cout << '\n';
            }
            std::cout << "Press Enter..."; std::getline(std::cin,line);
            continue;
        }
        if (line=="reset") { g.reset(); continue; }
        if (line=="undo") {
            if (g.canUndo()) { g.undo(); continue; }
            else { std::cout << "Nothing to undo. Press Enter..."; std::getline(std::cin,line); continue; }
        }

        int sr,sc,tr,tc; char promo=0;
        if (!parseMoveStr(line, sr,sc,tr,tc,promo)) { std::cout << "Couldn't parse move. Press Enter..."; std::getline(std::cin,line); continue; }
        if (!g.makeMoveIfLegal(sr,sc,tr,tc,promo)) { std::cout << "Illegal move. Press Enter..."; std::getline(std::cin,line); continue; }
    }

    std::cout << "Press Enter to exit..."; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}