#pragma once

#include <chrono>
#include <fstream>
#include <atomic>
#include <thread>
#include <mutex>

#include "chess.hpp"
#include "polyglotReader.h"
#include "tTable.h"
#include "evaluation.h"

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
        , _eval{Evaluation::staticEval(_board)}
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

    //PARAMETERS
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
    int negamax(int ply, int alpha, int beta);
    int quiescence(int alpha, int beta);
    void scoreMoves(chess::Movelist& moves, bool quiescent);
    void sortMoves(chess::Movelist& moves, int ind);

};