#pragma once

#include <chrono>
#include <fstream>
#include <atomic>
#include <thread>
#include <mutex>

#include "chess.hpp"
#include "polyglotReader.h"
#include "tTable.h"

class Engine {
public:

    struct SearchStatistics {
        int nodesSearched{0};
        int quiescenceNodes{0};
        int time{0};
        int numCutoffs{0};
        int depthSearched{0};
        int tableHits{0};
        
        int eval{0};
        bool forcedMate{false};
        chess::Move bestMove{};
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
    SearchStatistics getRuntimeStats();
    bool isSearching() {return _isSearching;}
    bool runtimeStatsAvailable() {return _runtimeStatsAvailable;}

    //Setters
    void newGame();
    void setBoard(chess::Board board);
    void useOpeningBook(std::string_view fileName);
    void collectStats(bool collectStats) {_collectStats = collectStats;}
    void setTableSize(int size) {_table.resize(size);}

    void clearTable() {_table.clear();}
    void finishSearching();
    void logStats(std::string_view logFileName);
    void stopLogStats() {_logStats = false;}

    //Engine commands
    void makeMove(const chess::Move& move);
    void thinkToPly(int maxPly);
    void think(int mili);

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
    TTable _table{1};

    //CONCURRENCY 
    std::thread _thinkThread{};
    std::thread _timerThread{};
    std::atomic_bool _stopSearching{false};
    std::atomic_bool _isSearching{false};
    std::atomic_bool _runtimeStatsAvailable{false};
    std::mutex _runtimeStatsMutex{};

    void thinkWorker(int maxPly);
    void timerWorker(int mili);

    //STATISTICS
    bool _collectStats{true};
    bool _logStats{false};
    std::string _logFileName{};
    SearchStatistics _stats{};
    SearchStatistics _runtimeStats{};
    
    void logStatsToFile();

    //OPENING BOOK READER
    std::optional<PolyglotReader> _openingBook{std::nullopt};
    
    inline void getBookMoves(chess::Movelist& moves) {_openingBook.value().getMoves(moves, _board);}

    //FUNCTIONS
    int staticEval();
    int negamax(int ply, int alpha, int beta);
    int quiescence(int alpha, int beta);
    void scoreMoves(chess::Movelist& moves, bool quiescent);
    void sortMoves(chess::Movelist& moves, int ind);

};