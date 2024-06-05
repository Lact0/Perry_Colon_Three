#include <iostream>
#include <fstream> 
#include <array>
#include <chrono>
#include <stdlib.h>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"
#include "inputHandler.h"



int main() {

    Engine engine{};
    engine.useOpeningBook("Titans.bin");
    // engine.logStats("Log Files/6-4-24#1.txt");

    InputHandler inp{};

    const chess::Board& engineBoard = engine.getBoard();
    const Engine::SearchStatistics& stats = engine.getSearchStats();

    bool searching = false;
    inp.start();
    while(true) {
        if(inp.inputRecieved()) {
            std::string input = inp.getInput();
            if(input == "go" && !engine.isSearching()) {
                engine.think(10000);
                searching = true;
            }
            if(input == "stop") {
                engine.finishSearching();
                searching = false;
                std::cout << "BESTMOVE: " << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << "\n";
            }
            if(input == "do") {
                engine.makeMove(engine.getBestMove());
            }
        }
        if(searching && !engine.isSearching()) {
            engine.finishSearching();
            searching = false;
            std::cout << "BESTMOVE: " << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << "\n";
        }
    }

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

    std::cout << "GAME OVER." << static_cast<int>(engineBoard.isGameOver().first);
}