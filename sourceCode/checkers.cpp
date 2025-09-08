#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <utility>
#include <limits>

using Pos = std::pair<int,int>; // row, col

static void clearScreen() { std::system("cls"); }

static bool inBounds(int r,int c){ return r>=0 && r<8 && c>=0 && c<8; }

static char pieceChar(int p){
    switch(p){
        case 1: return 'r'; // red man
        case 2: return 'R'; // red king
        case -1: return 'b'; // black man
        case -2: return 'B'; // black king
        default: return '.';
    }
}

static bool isOpponent(int p, int curPlayer){
    if (p==0) return false;
    return (p>0) != (curPlayer>0);
}

// find capture sequences for a single piece (recursive)
static void findCaptures(const std::vector<std::vector<int>>& board, int r, int c, 
                         int player, std::vector<Pos> path, std::vector<std::vector<Pos>>& out, 
                         std::vector<std::vector<int>> bcopy = {}, bool promoted = false)
{
    if (bcopy.empty()) bcopy = board;
    int piece = bcopy[r][c];
    bool king = (std::abs(piece) == 2) || promoted;
    bool any = false;
    const int drs[4] = {-1,-1,1,1};
    const int dcs[4] = {-1,1,-1,1};
    for (int k=0;k<4;++k){
        int dr = drs[k], dc = dcs[k];
        // men only move forward (toward opponent) unless king
        if (!king){
            if (player>0 && dr>0) continue; // red moves up (-1)
            if (player<0 && dr<0) continue; // black moves down (+1)
        }
        int mr = r+dr, mc = c+dc;
        int lr = r+2*dr, lc = c+2*dc;
        if (!inBounds(mr,mc) || !inBounds(lr,lc)) continue;
        if (!isOpponent(bcopy[mr][mc], player)) continue;
        if (bcopy[lr][lc] != 0) continue;
        // perform capture on copy
        auto nb = bcopy;
        nb[mr][mc] = 0;
        nb[lr][lc] = nb[r][c];
        nb[r][c] = 0;
        bool willPromote = promoted;
        // promote if lands on last row
        if (player>0 && lr==0) { nb[lr][lc] = 2; willPromote = true; }
        if (player<0 && lr==7) { nb[lr][lc] = -2; willPromote = true; }
        auto npath = path;
        npath.emplace_back(lr,lc);
        findCaptures(nb, lr, lc, player, npath, out, nb, willPromote);
        any = true;
    }
    if (!any && path.size()>1){
        // no further captures; record sequence
        out.push_back(path);
    }
}

// find single-step moves for a piece (non-captures)
static std::vector<std::vector<Pos>> findMoves(const std::vector<std::vector<int>>& board, int r, int c, int player){
    std::vector<std::vector<Pos>> ret;
    int piece = board[r][c];
    bool king = (std::abs(piece)==2);
    const int drs[4] = {-1,-1,1,1};
    const int dcs[4] = {-1,1,-1,1};
    for (int k=0;k<4;++k){
        int dr=drs[k], dc=dcs[k];
        if (!king){
            if (player>0 && dr>0) continue; // red moves up
            if (player<0 && dr<0) continue; // black moves down
        }
        int nr=r+dr, nc=c+dc;
        if (!inBounds(nr,nc)) continue;
        if (board[nr][nc]==0){
            ret.push_back({{r,c},{nr,nc}});
        }
    }
    return ret;
}

static bool hasAnyCaptures(const std::vector<std::vector<int>>& board, int player){
    for (int r=0;r<8;++r) for (int c=0;c<8;++c){
        if (board[r][c]==0) continue;
        if ((board[r][c]>0) != (player>0)) continue;
        std::vector<std::vector<Pos>> out;
        std::vector<Pos> p = {{r,c}};
        findCaptures(board,r,c,player,p,out);
        if (!out.empty()) return true;
    }
    return false;
}

static void printBoard(const std::vector<std::vector<int>>& board){
    clearScreen();
    std::cout << "   A B C D E F G H\n";
    for (int r=0;r<8;++r){
        std::cout << (r+1<10? " ":"") << r+1 << " ";
        for (int c=0;c<8;++c){
            char ch = pieceChar(board[r][c]);
            std::cout << ch << ' ';
        }
        std::cout << '\n';
    }
}

// parse input like "B6 C5" or "B6:C5:D3"
static bool parsePath(const std::string& s, std::vector<Pos>& out){
    std::string t;
    for (char ch : s) if (!isspace((unsigned char)ch)) t.push_back(ch);
    if (t.empty()) return false;
    // split by ':' or ','
    std::vector<std::string> parts;
    std::string cur;
    for (char ch : t){
        if (ch==':' || ch==',') { if(!cur.empty()) { parts.push_back(cur); cur.clear(); } }
        else cur.push_back(ch);
    }
    if (!cur.empty()) parts.push_back(cur);
    // if only two tokens without colon like "B6C5" attempt to split in middle
    if (parts.size()==1 && parts[0].size()==4){
        parts = { parts[0].substr(0,2), parts[0].substr(2,2) };
    }
    for (auto &p: parts){
        if (p.size()<2) return false;
        char col = p[0];
        char rowch = p[1];
        if (!isalpha((unsigned char)col) || !isdigit((unsigned char)rowch)) return false;
        int c = toupper(col)-'A';
        int r = (p.size()==3 && isdigit((unsigned char)p[2])) ? ( (p[1]-'0')*10 + (p[2]-'0') -1 ) : (p[1]-'0' -1);
        // handle rows 1..8
        if (r<0 || r>7 || c<0 || c>7) return false;
        out.emplace_back(r,c);
    }
    return out.size()>=2;
}

static void applyMove(std::vector<std::vector<int>>& board, const std::vector<Pos>& path, int player){
    Pos from = path.front();
    int piece = board[from.first][from.second];
    board[from.first][from.second] = 0;
    for (size_t i=1;i<path.size();++i){
        Pos to = path[i];
        int dr = to.first - from.first;
        int dc = to.second - from.second;
        if (std::abs(dr)==2 && std::abs(dc)==2){
            int mr = from.first + dr/2, mc = from.second + dc/2;
            board[mr][mc] = 0; // capture
        }
        from = to;
    }
    Pos last = path.back();
    // promote?
    if (player>0 && last.first==0) piece = 2;
    if (player<0 && last.first==7) piece = -2;
    board[last.first][last.second] = piece;
}

static bool anyLegalMoveForPlayer(const std::vector<std::vector<int>>& board, int player){
    if (hasAnyCaptures(board, player)) return true;
    for (int r=0;r<8;++r) for (int c=0;c<8;++c){
        if (board[r][c]==0) continue;
        if ((board[r][c]>0) != (player>0)) continue;
        auto moves = findMoves(board,r,c,player);
        if (!moves.empty()) return true;
    }
    return false;
}

int main(){
    // initialize board
    std::vector<std::vector<int>> board(8, std::vector<int>(8,0));
    for (int r=0;r<3;++r){
        for (int c=0;c<8;++c){
            if ((r+c)%2==1) board[r][c] = -1; // black top
        }
    }
    for (int r=5;r<8;++r){
        for (int c=0;c<8;++c){
            if ((r+c)%2==1) board[r][c] = 1; // red bottom
        }
    }

    int player = -1; // black starts (-1). Use -1 for black, +1 for red
    std::string line;
    while (true){
        printBoard(board);
        std::cout << (player<0 ? "Black (b/B)":"Red (r/R)") << " to move.\n";
        if (!anyLegalMoveForPlayer(board, player)){
            std::cout << (player<0 ? "Black":"Red") << " has no moves. ";
            std::cout << (player<0 ? "Red wins.\n":"Black wins.\n");
            break;
        }
        bool mustCapture = hasAnyCaptures(board, player);
        std::cout << "Enter move (e.g. B6 C5 or B6:C5:D3). ";
        if (mustCapture) std::cout << "You must capture.\n";
        else std::cout << '\n';
        std::getline(std::cin, line);
        if (line.empty()) continue;
        if (line=="q"||line=="Q") break;
        std::vector<Pos> path;
        if (!parsePath(line, path)){ std::cout << "Invalid input format. Press Enter to continue..."; std::getline(std::cin,line); continue; }
        // validate that starting square has player's piece
        Pos start = path.front();
        int piece = board[start.first][start.second];
        if (piece==0 || ((piece>0) != (player>0))){ std::cout << "No your piece at start. Press Enter..."; std::getline(std::cin,line); continue; }
        // determine if move is a valid capture sequence or simple move
        bool isCapture = false;
        bool legal = false;
        // try captures list for this piece
        std::vector<std::vector<Pos>> captures;
        std::vector<Pos> pinit = {start};
        findCaptures(board, start.first, start.second, player, pinit, captures);
        if (!captures.empty()){
            // normalize captures by comparing paths
            for (auto &cap : captures){
                if (cap == path){ legal = true; isCapture = true; break; }
            }
        }
        if (!legal && !mustCapture){
            // allow simple move check
            auto moves = findMoves(board, start.first, start.second, player);
            for (auto &m : moves){
                if (m == path){ legal = true; isCapture = false; break; }
            }
        }
        if (!legal){
            std::cout << "Illegal move. Press Enter to continue..."; std::getline(std::cin,line); continue;
        }
        // apply move
        applyMove(board, path, player);
        // if capture and further capture available from landing, enforce multi-jump when user didn't provide full sequence
        if (isCapture){
            Pos last = path.back();
            std::vector<std::vector<Pos>> more;
            std::vector<Pos> p2 = { last };
            findCaptures(board, last.first, last.second, player, p2, more);
            if (!more.empty()){
                // user must continue capturing; simple approach: allow multiple separate inputs (turn remains with same player)
                printBoard(board);
                std::cout << "Continue capture from " << char('A'+last.second) << (last.first+1) << ". Enter continuation path (e.g. " 
                          << char('A'+last.second) << (last.first+1) << ":D4) or full chain originally.\n";
                std::getline(std::cin,line);
                if (line.empty()){ /* skip continue and keep turn */ }
                else {
                    std::vector<Pos> cont;
                    if (parsePath(line, cont)){
                        // if cont starts with same start, apply full cont (user could give entire chain)
                        if (cont.front()==path.front()){
                            applyMove(board, cont, player);
                        } else if (cont.front()==last){
                            // build combined path
                            std::vector<Pos> combo = path;
                            combo.insert(combo.end(), cont.begin()+1, cont.end());
                            applyMove(board, combo, player);
                        }
                    }
                }
            }
        }
        // switch turn
        player = -player;
    }

    std::cout << "Game over. Press Enter to exit...";
    std::getline(std::cin,line);
    return 0;
}