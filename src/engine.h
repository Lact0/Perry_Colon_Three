#pragma once

#include "chess.hpp"



class Engine {
public:

    struct Parameters {
        int pieceValues[7] {100, 320, 330, 500, 900, 20000, 0};
    };

    Engine()
        : _board{}
        , _bestMove{}
    {}

    Engine(chess::Board board)
        : _board{board}
        , _bestMove{}
    {}

    void setBoard(chess::Board board);
    
    int getEval() {return _eval;}
    chess::Board getBoard() {return _board;}
    chess::Move getBestMove() {return _bestMove;}

    void makeMove(const chess::Move& move);

    void think(int maxPly);

private:

    static constexpr int _pInf{1000000};
    static constexpr int _nInf{-1000000};

    int _eval{0};
    chess::Board _board{};
    chess::Move _bestMove{};

    Parameters _params{};

    int negamax(int ply);
    int staticEval();

};