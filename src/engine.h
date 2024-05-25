#pragma once

#include "chess.hpp"



class Engine {
public:

    Engine()
        : _board{}
        , _bestMove{}
    {}

    Engine(chess::Board board)
        : _board{board}
        , _bestMove{}
    {}

    void setBoard(chess::Board board);
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

    int negamax(int ply);
    int staticEval();

};