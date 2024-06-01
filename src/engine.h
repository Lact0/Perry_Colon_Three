#pragma once

#include <chrono>
#include <fstream>

#include "chess.hpp"
#include "polyglotReader.h"

class Engine {
public:

    struct SearchStatistics {
        int nodesSearched{0};
        int time{0};
        int numCutoffs{0};
        int depthSearched{0};
    };

    //Constructors
    Engine() = default;
    Engine(chess::Board board)
        : _board{board}
        , _eval{staticEval()}
    {}

    //Getters
    int getEval() {return _eval;}
    const chess::Board& getBoard() {return _board;}
    const chess::Move& getBestMove() {return _bestMove;}
    const SearchStatistics& getSearchStats() {return _stats;}

    //Setters
    void setBoard(chess::Board board);
    void useOpeningBook(std::string_view fileName);
    void collectStats(bool collectStats) {_collectStats = collectStats;}

    void logStats(std::string_view logFileName);
    void stopLogStats() {_logStats = false;}

    //Engine commands
    void makeMove(const chess::Move& move);
    void think(int maxPly);

private:

    //CONSTANTS
    static constexpr int _pInf{1000000};
    static constexpr int _nInf{-1000000}; 

    static constexpr int _pieceSquareTable[6][64] = {
        { // PAWN
                0,   0,   0,   0,   0,   0,   0,   0,
                5,  10,  10, -20, -20,  10,  10,   5,
                5,  -5, -10,   0,   0, -10,  -5,   5,
                0,   0,   0,  20,  20,   0,   0,   0,
                5,   5,  10,  25,  25,  10,   5,   5,
                10,  10,  20,  30,  30,  20,  10,  10,
                50,  50,  50,  50,  50,  50,  50,  50,
                0,   0,   0,   0,   0,   0,   0,   0
        },
        { // HORSE
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20,   0,   0,   0,   0, -20, -40,
            -30,   0,  10,  15,  15,  10,   0, -30,
            -30,   0,  15,  20,  20,  15,   0, -30,
            -30,   0,  15,  20,  20,  15,   0, -30,
            -30,   0,  10,  15,  15,  10,   0, -30,
            -40, -20,   0,   0,   0,   0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50
        },
        { //BISHOP
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10,   5,   0,   0,   0,   0,   5, -10,
            -10,  10,  10,  10,  10,  10,  10, -10,
            -10,   0,  10,  10,  10,  10,   0, -10,
            -10,   5,   5,  10,  10,   5,   5, -10,
            -10,   0,   5,  10,  10,   5,   0, -10,
            -10,   0,   0,   0,   0,   0,   0, -10,
            -20, -10, -10, -10, -10, -10, -10, -20
        },
        { //ROOK
                0,   0,   0,   5,   5,   0,   0,   0,
                -5,   0,   0,   0,   0,   0,   0,  -5,
                -5,   0,   0,   0,   0,   0,   0,  -5,
                -5,   0,   0,   0,   0,   0,   0,  -5,
                -5,   0,   0,   0,   0,   0,   0,  -5,
                -5,   0,   0,   0,   0,   0,   0,  -5,
                5,  10,  10,  10,  10,  10,  10,   5,
                0,   0,   0,   0,   0,   0,   0,   0
        },
        { //QUEEN
            -20, -10, -10,  -5,  -5, -10, -10, -20,
            -10,   0,   0,   0,   0,   0,   0, -10,
            -10,   0,   5,   5,   5,   5,   0, -10,
                0,   0,   5,   5,   5,   5,   0,  -5,
                -5,   0,   5,   5,   5,   5,   0,  -5,
            -10,   5,   5,   5,   5,   5,   0, -10,
            -10,   0,   5,   0,   0,   0,   0, -10,
            -20, -10, -10,  -5,  -5, -10, -10, -20
        },
        { //KING
                20,  30,  10,   0,   0,  10,  30,  20,
                20,  20,   0,   0,   0,   0,  20,  20,
            -10, -20, -20, -20, -20, -20, -20, -10,
            -20, -30, -30, -40, -40, -30, -30, -20,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30
        }

    };

    //PARAMETERS
    int _pieceValues[5] {100, 320, 330, 500, 900};
    bool _useOpeningBook{false};

    //VARIABLE MEMBERS
    chess::Board _board{};
    chess::Move _bestMove{};
    int _eval{0};

    //STATISTICS
    bool _collectStats{true};
    bool _logStats{false};
    SearchStatistics _stats{};
    std::string _logFileName{};

    std::optional<PolyglotReader> _openingBook{std::nullopt};

    int staticEval();
    inline void getBookMoves(chess::Movelist& moves) {_openingBook.value().getMoves(moves, _board);}
    void logStatsToFile();

    int negamax(int ply, int alpha, int beta);

};