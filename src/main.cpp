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
    std::cout << "bestmove " + bestMove + "\n" << std::flush;
}

void runUCI() {

    //Send init info
    std::cout << "id name Perry Colon Three\n";
    std::cout << "id author Peter Perry\n";
    
    //TODO: GIVE AND RECIEVE OPTIONS
    std::cout << "option name Hash type spin default 1 min 1 max 128\n";
    std::cout << "option name ClearHash type button\n";
    std::cout << "uciok\n";
    
    //Option settings
    int tableSize = 1;
    
    //Custom settings
    bool debugOn = false;
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

        if(cmd[0] == "setoption") {
            if(cmd[2] == "Hash") {
                tableSize = std::stoi(cmd[4]);
            }
        }

        if(cmd[0] == "debugOn") {
            debugOn = true;
        }
    }
    
    Engine engine{};

    if(useOutsideOpeningBook) engine.useOpeningBook(bookFileName);
    if(logToFile) engine.logStats(logFileName);
    engine.setTableSize(tableSize);

    if(debugOn) {
        std::cout << "DEBUG: Table Size set to " << tableSize << "\n" << std::flush;
        if(useOutsideOpeningBook) std::cout << "DEBUG: Using opening book " << bookFileName << "\n" << std::flush;
        if(logToFile) std::cout << "DEBUG: Logging to file " << logFileName << "\n" << std::flush;
    }

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

            if(cmd.size() == 0) {
                if(debugOn) std::cout << "DEBUG: EMTPY INPUT, IGNORING\n" << std::flush;
            
            } else if(inp == "ucinewgame") {
            
                engine.setBoard(chess::Board());
                if(debugOn) std::cout << "DEBUG: New game set.\n" << std::flush;
            
            } else if(cmd[0] == "position") {

                engine.setBoard(parsePositionCmd(cmd));
                if(debugOn) std::cout << "DEBUG: Position command recieved.\n" << std::flush;
            
            } else if(cmd[0] == "go") {

                if(debugOn) std::cout << "DEBUG: Go command recieved.\n" << std::flush;
                if(cmd.size() == 1) {
                    engine.thinkToPly(100);
                    isThinking = true;
                    if(debugOn) std::cout << "DEBUG: GO COMMAND WITH ONLY ONE ARG WAS GIVEN\n" << std::flush;
                } else if(cmd[1] == "infinite") {
                    engine.thinkToPly(100);
                    isThinking = true;
                } else if(cmd[1] == "depth") {
                    engine.thinkToPly(std::stoi(cmd[2]));
                    isThinking = true;
                } else if(cmd[1] == "movetime") {
                    engine.think(std::stoi(cmd[2]));
                    isThinking = true; 
                }

            } else if(cmd[0] == "setoption") {

                if(cmd[2] == "ClearHash") {
                    engine.clearTable();
                    if(debugOn) std::cout << "DEBUG: TTable cleared.\n" << std::flush;
                }

            } else if(cmd[0] == "stop") {
                finishThinking(engine);
                isThinking = false;
                if(debugOn) std::cout << "DEBUG: Engine has stopped thinking.\n" << std::flush;
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
        engine.think(mili);
        while(engine.isSearching()) {}
        engine.finishSearching();

        std::cout << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << " " << engine.getEval() 
                    << " " << stats.time << "ms " << stats.nodesSearched << " "
                    << stats.numCutoffs << " " << stats.depthSearched << "\n";
        
        engine.makeMove(engine.getBestMove());
    }
    std::cout << "GAME OVER:" << static_cast<int>(engineBoard.isGameOver().first) << 
        " " << static_cast<int>(engineBoard.sideToMove()) << "\n";
}

int main() {

    std::string inp = "";

    while(inp != "exit") {
        std::getline(std::cin, inp);

        std::vector<std::string> cmd = splitString(inp);

        if(cmd[0] == "uci") runUCI();
        else if(cmd[0] == "selfgame") runGame(std::stoi(cmd[1]));
    }


    // Engine engine{};
    // engine.useOpeningBook("Titans.bin");
    // // engine.logStats("Log Files/6-4-24#1.txt");

    // InputHandler inputHandler{};

    // const chess::Board& engineBoard = engine.getBoard();
    // const Engine::SearchStatistics& stats = engine.getSearchStats();

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