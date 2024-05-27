#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"

int main() {

    Engine engine{chess::Board("8/8/8/p3p3/P2n4/7k/KP6/3q4 w - - 5 59")};
    engine.useOpeningBook("Titans.bin");
    // engine.setVerbose(true);

    while(engine.getBoard().isGameOver().second == chess::GameResult::NONE) {

        engine.think(4);
        std::cout << chess::uci::moveToSan(engine.getBoard(), engine.getBestMove()) << " " << engine.getEval() << "\n";
        engine.makeMove(engine.getBestMove());
    }

    std::cout << "GAME OVER.";
}