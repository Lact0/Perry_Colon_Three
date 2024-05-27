#pragma once

#include "chess.hpp"
#include "polyglotReader.h"


class Engine {
public:

    Engine() = default;

    Engine(chess::Board board)
        : _board{board}
        , _eval{staticEval()}
    {}

    void setBoard(chess::Board board);
    
    int getEval() {return _eval;}
    chess::Board getBoard() {return _board;}
    chess::Move getBestMove() {return _bestMove;}

    void useOpeningBook(std::string_view fileName);

    void makeMove(const chess::Move& move);
    void think(int maxPly);

private:

    //CONSTANTS
    static constexpr int _pInf{1000000};
    static constexpr int _nInf{-1000000};

    //PARAMETERS
    int _pieceValues[5] {100, 320, 330, 500, 900};
    bool _useOpeningBook{false};

    //VARIABLE MEMBERS
    chess::Board _board{};
    chess::Move _bestMove{};
    int _eval{0};

    std::optional<PolyglotReader> _openingBook{std::nullopt};

    int negamax(int ply);
    int staticEval();
    inline void getBookMoves(chess::Movelist& moves) {_openingBook.value().getMoves(moves, _board);}

};