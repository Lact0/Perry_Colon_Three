#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"


PolyglotReader bookReader{"Titans.bin"};

int main() {

    Engine engine{};

    engine.think(5);

    std::cout << chess::uci::moveToSan(engine.getBoard(), engine.getBestMove());

    // chess::Board board{};

    // while(true) {
    //     chess::Movelist moves{};
    //     bookReader.getMoves(moves, board);

    //     if(moves.size() == 0) {
    //         std::cout << "FINISHED.";
    //         break;
    //     }

    //     chess::Move& move = moves[0];
    //     std::cout << chess::uci::moveToSan(board, move) << "\n";
    //     board.makeMove(move); 
    // }

}