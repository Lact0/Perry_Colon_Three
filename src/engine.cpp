#include "engine.h"

void Engine::setBoard(chess::Board board) {
    _board = board;
    _bestMove = chess::Move();
}

void Engine::makeMove(const chess::Move& move) {
    _board.makeMove(move);
    _bestMove = chess::Move();
}

void Engine::think(int maxPly) {

    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);

    for(int ply{0}; ply < maxPly; ++ply) {

        int bestEval{_nInf};

        for(const chess::Move& move: moves) {
            
            _board.makeMove(move);
            int curEval{negamax(ply)};
            _board.unmakeMove(move);

            if(curEval > bestEval) {
                bestEval = curEval;
                _eval = curEval;
                _bestMove = move;
            }

        }

    }

}

int Engine::negamax(int ply) {

    int sideToMove{_board.sideToMove() == chess::Color::WHITE ? 1 : -1};
    if(ply == 0) return staticEval() * sideToMove;

    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);
    
    int bestEval{_nInf};

    for(const chess::Move& move: moves) {
        
        _board.makeMove(move);
        int curEval{-negamax(ply - 1)};
        _board.unmakeMove(move);

        if(curEval > bestEval) bestEval = curEval;

    }

    return bestEval;
}

int Engine::staticEval() {
    return 0;
}