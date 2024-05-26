#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"
#include "engine.h"


PolyglotReader bookReader{"Titans.bin"};

int main() {

    Engine engine{chess::Board("rnb1kbnr/ppp1pppp/8/3p4/4P1Q1/3P2q1/PPP2PPP/RNB1KBNR b KQkq - 0 5")};

    for(int i = 1; i <= 5; i++) {
    engine.think(i);

    std::cout << chess::uci::moveToSan(engine.getBoard(), engine.getBestMove()) << " " << engine.getEval() << "\n";
    }

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