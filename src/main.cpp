#include <iostream>
#include <fstream> 
#include <array>
#include <chrono>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"



int main() {

    Engine engine{};
    engine.useOpeningBook("Titans.bin");
    // engine.logStats("Log Files/6-1-24#2.txt");

    const chess::Board& engineBoard = engine.getBoard();
    const Engine::SearchStatistics& stats = engine.getSearchStats();

    while(engineBoard.isGameOver().second == chess::GameResult::NONE) {

        if(false && engineBoard.sideToMove() == chess::Color::WHITE) {
            std::string sanMove;
            std::cin >> sanMove;
            engine.makeMove(chess::uci::parseSan(engineBoard, sanMove));

        } else {

            engine.think(10);

            int searchTime = 100;
            auto startTime = std::chrono::high_resolution_clock::now();

            while(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count() < searchTime
                || !engine.isSearching()) {}
            engine.stopSearching();



            std::cout << chess::uci::moveToSan(engineBoard, engine.getBestMove()) << " " << engine.getEval() 
                      << " " << stats.time << "ms " << stats.nodesSearched << " "
                      << stats.numCutoffs << " " << stats.depthSearched << "\n";
            
            engine.makeMove(engine.getBestMove());
        }
    }

    std::cout << "GAME OVER." << static_cast<int>(engineBoard.isGameOver().first);
}