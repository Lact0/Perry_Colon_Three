#include <iostream>
#include <fstream> 
#include <array>

#include "chess.hpp"
#include "polyglotReader.h"


PolyglotReader bookReader{"Titans.bin"};

int main() {

    chess::Board board{"rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2"};

    chess::Movelist moves{};
    bookReader.getMoves(moves, board);
    for(const chess::Move& move: moves) {
        std::cout << chess::uci::moveToSan(board, move) << " " << move.score() << "\n";
    }   

}