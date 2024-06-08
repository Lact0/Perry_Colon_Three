#include <iostream>
#include <fstream> 
#include <chrono>
#include <vector>
#include <string>

#include "chess.hpp"
#include "engine.h"
#include "inputHandler.h"

std::vector<std::string> splitString(std::string_view str, const char d = ' ') {

    std::vector<std::string> ret{};
    std::string curString = "";
    
    for(int i = 0; i < str.size(); i++) {
        char c = str.at(i);
        if(c == d && curString.size() > 0) {
            ret.push_back(curString);
            curString = "";
        } else if (c != d) {
            curString += c;
        }
    }

    if(curString.size() > 0) {
        ret.push_back(curString);
    }

    return ret;
}

chess::Board parsePositionCmd(std::vector<std::string>& movelist) {
    
    int moveIndex = -1;
    for(int i = 1; i < movelist.size(); i++) {
        if(movelist[i] == "moves") {
            moveIndex = i;
            break;
        }
    }

    if(moveIndex == -1) {
        moveIndex = movelist.size();
    }

    chess::Board newBoard;
    if(movelist[1] == "startpos") {
        newBoard = chess::Board();
    } else {
        std::string fen = movelist[1];

        for(int i = 2; i < moveIndex; i++) {
            fen += " " + movelist[i];
        }

        newBoard = chess::Board(fen);
    }

    for(int i = moveIndex + 1; i < movelist.size(); i++) {
        std::string move = movelist[i];
        newBoard.makeMove(chess::uci::uciToMove(newBoard, move));
    }

    return newBoard;
}

void finishThinking(Engine& engine) {
    engine.finishSearching();
    std::string bestMove = chess::uci::moveToUci(engine.getBestMove());
    std::cout << "bestmove " + bestMove + "\n";
}

void runUCI() {

    //Send init info
    std::cout << "id name Perry Colon Zero\n";
    std::cout << "id author Peter Perry\n";
    
    //TODO: GIVE AND RECIEVE OPTIONS
    std::cout << "uciok\n";

    bool useOutsideOpeningBook = false;
    std::string bookFileName;

    bool logToFile = false;
    std::string logFileName;

    //Parse gui options
    std::string inp = "";
    while(inp != "isready") {
        std::getline(std::cin, inp);
        std::vector<std::string> cmd = splitString(inp);
        
        if(cmd[0] == "useOutsideOpeningBook") {
            useOutsideOpeningBook = true;
            bookFileName = cmd[1];
        }

        if(cmd[0] == "logToFile") {
            logToFile = true;
            logFileName = cmd[1];
            for(int i = 2; i < cmd.size(); i++) {
                logFileName += " " + cmd[i];
            }
            std::cout << logFileName << "\n";
        }
    }
    
    Engine engine{};

    if(useOutsideOpeningBook) engine.useOpeningBook(bookFileName);
    if(logToFile) engine.logStats(logFileName);

    std::cout << "readyok\n";

    bool isThinking = false;

    InputHandler inputHandler{};
    inputHandler.start();

    while(true) {

        if(isThinking && !engine.isSearching()) {
            finishThinking(engine);
            isThinking = false;
        }

        if(inputHandler.inputRecieved()) {
            inp = inputHandler.getInput();
            std::vector<std::string> cmd = splitString(inp);

            if(inp == "ucinewgame") {
                engine.setBoard(chess::Board());
            }

            if(cmd[0] == "position") {
                engine.setBoard(parsePositionCmd(cmd));
            }

            if(cmd[0] == "go") {
                
                if(cmd[1] == "infinite") {
                    engine.thinkToPly(100);
                    isThinking = true;
                }
                
                if(cmd[1] == "depth") {
                    engine.thinkToPly(std::stoi(cmd[2]));
                    isThinking = true;
                }

                if(cmd[1] == "movetime") {
                    engine.think(std::stoi(cmd[2]));
                    isThinking = true; 
                }

            } 
            
            if(cmd[0] == "stop") {
                finishThinking(engine);
                isThinking = false;
            }

        }
    }
}

void runGame(int mili) {
    Engine engine{};
    engine.useOpeningBook("Titans.bin");
    // engine.logStats("Log Files/6-4-24#1.txt");

    InputHandler inputHandler{};

    const chess::Board& engineBoard = engine.getBoard();
    const Engine::SearchStatistics& stats = engine.getSearchStats();

    while(engineBoard.isGameOver().second == chess::GameResult::NONE) {
        if(false && engineBoard.sideToMove() == chess::Color::WHITE) {
            std::string sanMove;
            std::cin >> sanMove;
            engine.makeMove(chess::uci::parseSan(engineBoard, sanMove));

        } else {

            engine.think(mili);
            while(engine.isSearching()) {}
            engine.finishSearching();

            std::cout << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << " " << engine.getEval() 
                      << " " << stats.time << "ms " << stats.nodesSearched << " "
                      << stats.numCutoffs << " " << stats.depthSearched << "\n";
            
            engine.makeMove(engine.getBestMove());
        }
    }
    std::cout << "GAME OVER:" << static_cast<int>(engineBoard.isGameOver().first) << 
        " " << static_cast<int>(engineBoard.sideToMove());
}

int main() {

    std::string inp;
    std::getline(std::cin, inp);

    std::vector<std::string> cmd = splitString(inp);

    if(cmd[0] == "uci") runUCI();
    else if(cmd[0] == "selfgame") runGame(std::stoi(cmd[1]));


    Engine engine{};
    engine.useOpeningBook("Titans.bin");
    // engine.logStats("Log Files/6-4-24#1.txt");

    InputHandler inputHandler{};

    const chess::Board& engineBoard = engine.getBoard();
    const Engine::SearchStatistics& stats = engine.getSearchStats();

    // bool searching = false;
    // inputHandler.start();
    // while(true) {
    //     if(inputHandler.inputRecieved()) {
    //         std::string input = inputHandler.getInput();
    //         if(input == "go" && !engine.isSearching()) {
    //             engine.think(10000);
    //             searching = true;
    //         }
    //         if(input == "stop") {
    //             engine.finishSearching();
    //             searching = false;
    //             std::cout << "BESTMOVE: " << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << "\n";
    //         }
    //         if(input == "do") {
    //             engine.makeMove(engine.getBestMove());
    //         }
    //     }
    //     if(searching && !engine.isSearching()) {
    //         engine.finishSearching();
    //         searching = false;
    //         std::cout << "BESTMOVE: " << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << "\n";
    //     }
    // }

    // while(engineBoard.isGameOver().second == chess::GameResult::NONE) {

    //     if(false && engineBoard.sideToMove() == chess::Color::WHITE) {
    //         std::string sanMove;
    //         std::cin >> sanMove;
    //         engine.makeMove(chess::uci::parseSan(engineBoard, sanMove));

    //     } else {

    //         engine.think(1000);
    //         while(engine.isSearching()) {}
    //         engine.finishSearching();

    //         std::cout << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << " " << engine.getEval() 
    //                   << " " << stats.time << "ms " << stats.nodesSearched << " "
    //                   << stats.numCutoffs << " " << stats.depthSearched << "\n";
            
    //         engine.makeMove(engine.getBestMove());
    //     }
    // }

    // std::cout << "GAME OVER." << static_cast<int>(engineBoard.isGameOver().first);

}