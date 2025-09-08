// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//  The include file for standard system include files,
//  or project specific include files.
#include <iostream>
#include <limits>
#include <filesystem>
#include <cstdlib>
#include <string>
//using the namespace std
using namespace std;
static int launchExe(const std::filesystem::path &relPath) {
    std::filesystem::path exe = std::filesystem::current_path() / relPath;
    std::string cmd = "\"" + exe.string() + "\"";
    return std::system(cmd.c_str());
}
//function prototypes
void gamblingInCpp();
void consoleRPG();
//main function
int main(){
    int c;
    int o;
    //introduction
    system("cls");
    std::cout<<"Welcome to ASYS\n";
    std::cout<<"Collections\n0.Credits\n1.Gambling in C++\n2.ConsoleRPG(demo)\n3.Math functions\n4.Minigames\n5.Chess\n";
    std::cin>>c;
    //switch case to launch the selected executable
    switch (c)
    {
    case 0:
        {
            //launch credits executable
            system("cls");
            int rc = launchExe("minorColection/Credits.exe");
            if(rc != 0) std::cout << "system() returned " << rc << "\n";
            break;
        }
    case 1:
    //launch gambling in C++ games menu
        gamblingInCpp();
        break;
    case 2:
    //launch console RPG demo
        consoleRPG();
        break;
    case 3:
    //launch math functions menu
        {   // <-- add braces to scope local variables
        //launch math functions menu
        system("cls");
        int rc = launchExe("minorColection/Math.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 4:
    //launch minigames menu
    int game;
        system("cls");
        std::cout<<"Minigames\n1.Memory Game\n2.Hangman\n3.Snake Game\n4.Minesweeper\n5.Guess the Number\n6.Connect 4\n7.Pong\n8.Battleship\n9.2048\n10.Checkers\n";
        std::cin>>game;
        switch (game){
            case 1:
                {
                    system("cls");
                    int rc = launchExe("miniGames/memory.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 2:
                {
                    system("cls");
                    int rc = launchExe("miniGames/hangman.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 3:
                {
                    system("cls");
                    int rc = launchExe("miniGames/snake.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 4:
                {
                    system("cls");
                    int rc = launchExe("miniGames/minesweeper.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 5:
                {
                    system("cls");
                    int rc = launchExe("miniGames/guessthenumber.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 6:
                {
                    system("cls");
                    int rc = launchExe("miniGames/connect4.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 7:
                {
                    system("cls");
                    int rc = launchExe("miniGames/pong.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 8:
                {
                    system("cls");
                    int rc = launchExe("miniGames/battleship.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 9:
                {
                    system("cls");
                    int rc = launchExe("miniGames/2048.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            case 10:
                {
                    system("cls");
                    int rc = launchExe("miniGames/checkers.exe");
                    if(rc != 0) std::cout << "system() returned " << rc << "\n";
                    break;
                }
            default:
                std::cout<<"Invalid option\n";
                break;
        }
    break;
    case 5:
    //launch chess game
    {
        int op;
        system("cls");
        std::cout<<"Chess\n1.Play vs Stockfish(UCI)\n2.Play vs Random AI\n3.Play vs Human(local)\n4.Return to main menu\n";
        std::cin>>op;
        switch (op){
        case 1:
            {
                system("cls");
                int rc = launchExe("minorColection/uci_chess.exe");
                if(rc != 0) std::cout << "system() returned " << rc << "\n";
                break;
            }
        case 2:
            {
                system("cls");
                int rc = launchExe("minorColection/chess.exe");
                if(rc != 0) std::cout << "system() returned " << rc << "\n";
                break;
            }
            case 3:
            {
                system("cls");
                int rc = launchExe("minorColection/chess.exe");
                if(rc != 0) std::cout << "system() returned " << rc << "\n";
                break;
            }
    }
}
    break;
    case 67:
    //Easter egg
        system("cls");
        std::cout<<"Sybau\n";
    break;
    case 539787:
    //Easter egg
    {
        system("cls");
        int op;
        std::cout<<"You found the secret code!\n";
        std::cout<<"You can now play Uno!\n";
        std::cout<<"1.Play Uno Singleplayer\n2.Play Uno Multiplayer(prson has to be next to you)\n3.Exit\n";
        std::cin>>op;
        switch (op){
        case 1:
            {
                system("cls");
                int rc = launchExe("minorColection/uno.exe");
                if(rc != 0) std::cout << "system() returned " << rc << "\n";
                break;
            }
        case 2:
            {
                system("cls");
                int rc = launchExe("minorColection/uno2player.exe");
                if(rc != 0) std::cout << "system() returned " << rc << "\n";
                break;
            }
        case 3:

        break;
        default:
            std::cout<<"Invalid option\n";
            break;
        }
    }
    break;
    system("cls");
    default:
    std::cout<<"Invalid option\n";
        break;
    }

    // wait for user to press Enter before closing
    std::cout << "\nPress Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}
void gamblingInCpp(){
    //function to launch gambling in C++ games
    int o;
    system("cls");
    std::cout<<"Gambling in C++\n";
    std::cout<<"1.Plinko\n2.Pool\n3.Poker\n4.Roulette\n5.Rock Paper Scissors\n6.Slot Machine\n7.Tic Tac Toe\n8.Crocodile Dentist\n";
    //get user input
    std::cin>>o;
    //switch case to launch the selected executable
    switch (o){
 case 1: {
        system("cls");
        int rc = launchExe("GamblingInC++/plinko.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 2: {
        system("cls");
        int rc = launchExe("GamblingInC++/pool.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 3: {
        system("cls");
        int rc = launchExe("GamblingInC++/poker.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 4: {
        system("cls");
        int rc = launchExe("GamblingInC++/roulette.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 5: {
        system("cls");
        int rc = launchExe("GamblingInC++/rockpaperscissors.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 6: {
        system("cls");
        int rc = launchExe("GamblingInC++/slotmachine.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 7: {
        system("cls");
        int rc = launchExe("GamblingInC++/tictactoe.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    case 8: {
        system("cls");
        int rc = launchExe("GamblingInC++/crocodiledentist.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
        break;
    }
    default:
        std::cout<<"Invalid option\n";
        break;
    }
}
void consoleRPG(){
    //run console RPG demo
    system("cls");
    std::cout<<"Console RPG\n";
    int rc = launchExe("ConsoleRPG/ConsoleRPG.exe");
        if(rc != 0) std::cout << "system() returned " << rc << "\n";
}