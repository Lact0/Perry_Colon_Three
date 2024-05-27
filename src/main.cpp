#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"

int main() {

    Engine engine{};
    engine.useOpeningBook("Titans.bin");

    for(int i = 1; i <= 5; i++) {
        engine.think(i);

        std::cout << chess::uci::moveToSan(engine.getBoard(), engine.getBestMove()) << " " << engine.getEval() << "\n";
    }

}