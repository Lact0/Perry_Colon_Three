#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"

int main() {

    Engine engine{};
    //engine.useOpeningBook("Titans.bin");
    engine.setVerbose(true);

    while(engine.getBoard().isGameOver().second == chess::GameResult::NONE) {
        engine.think(5);
        std::cout << chess::uci::moveToSan(engine.getBoard(), engine.getBestMove()) << " " << engine.getEval() << "\n";
        engine.makeMove(engine.getBestMove());
    }

    std::cout << "GAME OVER.";
}