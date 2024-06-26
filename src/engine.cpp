#include "engine.h"

Engine::SearchStatistics Engine::getRuntimeStats() {

    _runtimeStatsMutex.lock();
    _runtimeStatsAvailable = false;
    SearchStatistics ret = _runtimeStats;
    _runtimeStatsMutex.unlock();

    return ret;
}

void Engine::newGame() {
    setBoard(chess::Board());
}

void Engine::setBoard(chess::Board board) {
    _board = board;
    _bestMove = chess::Move();
    _eval = Evaluation::staticEval(board);
}

void Engine::useOpeningBook(std::string_view fileName) {
    _useOpeningBook = true;
    _openingBook.emplace(fileName);

}

void Engine::finishSearching() {
    _stopSearching = true;
    if(_thinkThread.joinable()) _thinkThread.join();
    if(_timerThread.joinable()) _timerThread.join();
}

void Engine::logStats(std::string_view logFileName) {
    _logStats = true;
    _logFileName = logFileName;
}

void Engine::makeMove(const chess::Move& move) {
    _board.makeMove(move);
    _bestMove = chess::Move();
    _eval = Evaluation::staticEval(_board);
}

void Engine::thinkToPly(int maxPly) {

    //Prevent from starting a second think thread
    if(_isSearching) return;

    //Setup concurrency variables
    _stopSearching = false;
    _isSearching = true;

    //Start thread
    _thinkThread = std::thread{&Engine::thinkWorker, this, maxPly};
}

void Engine::think(int mili) {

    if(_isSearching) return;

    _stopSearching = false;
    _isSearching = true;
    _runtimeStatsAvailable = false;

    _thinkThread = std::thread{&Engine::thinkWorker, this, 100};
    _timerThread = std::thread(&Engine::timerWorker, this, mili);
}

void Engine::thinkWorker(int maxPly) {

    using time = std::chrono::_V2::system_clock::time_point;
    time start;

    if(_collectStats) {
        start = std::chrono::high_resolution_clock::now();
        _stats = SearchStatistics{};
    }

    //Gen moves
    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);
    if(moves.size() > 0) _bestMove = moves[0];

    //Look up book moves
    chess::Movelist bookMoves{};
    if(_useOpeningBook) getBookMoves(bookMoves);
    bool bookMoveExists{bookMoves.size() > 0};

    //Iterative Deepening
    for(int ply{0}; ply < maxPly; ++ply) {
        if(bookMoveExists) break;

        //Best eval must be worse than checkmate
        int bestEval{_nInf - 1};
        chess::Move bestMove{};

        //Search through each move
        for(const chess::Move& move: moves) {
            
            _board.makeMove(move);
            int curEval{-negamax(ply, _nInf - 1, -bestEval)};
            _board.unmakeMove(move);

            if(curEval > bestEval) {
                bestEval = curEval;
                bestMove = move;
            }

        }

        //Handle for early exit
        if(_stopSearching) break;

        bool forcedMate = bestEval == _pInf || bestEval == _nInf;

        _stats.eval = _eval = bestEval;
        _stats.bestMove = _bestMove = bestMove;
        if(_collectStats) _stats.depthSearched = ply + 1;
        
        //Update score to show mate in # moves
        if(forcedMate) {
            _stats.forcedMate = true;
            _stats.eval = (ply + 1) / 2;
            if(bestEval < 0) _stats.eval *= -1;
        }

        //Update runtime Stats
        _runtimeStatsMutex.lock();
        _runtimeStatsAvailable = true;
        _runtimeStats = _stats;
        _runtimeStatsMutex.unlock();

        if(forcedMate) break;
    }

    //Bookmove trumps search
    if(bookMoveExists) {
        _stats.bestMove = _bestMove = bookMoves[0];
    }

    if(_collectStats) {
        time end = std::chrono::high_resolution_clock::now();
        _stats.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    if(_logStats) logStatsToFile();

    _isSearching = false;
}

void Engine::timerWorker(int mili) {
    auto start = std::chrono::high_resolution_clock::now();
    while(std::chrono::duration_cast<std::chrono::milliseconds>
         (std::chrono::high_resolution_clock::now() - start).count() < mili
         && !_stopSearching) {}
    
    _stopSearching = true;
}

int Engine::negamax(int ply, int alpha, int beta) {

    if(_stopSearching) return 0;

    //Check for repetitions
    if(_board.isRepetition(1)) return 0;
    if(_board.isHalfMoveDraw()) return 0;

    //Check table for valid entry
    int oldAlpha = alpha;
    if(_table.hasEntry(_board.hash())) {
        const TTable::Entry& entry = _table.getEntry(_board.hash());
        if(entry.depth >= ply) {
            if(_collectStats) ++_stats.tableHits;
            
            switch(entry.flag) {
            case TTable::Entry::EXACT: 
                return entry.eval;
            case TTable::Entry::LOWER:
                if(entry.eval > alpha) alpha = entry.eval; break;
            case TTable::Entry::UPPER:
                if(entry.eval < beta) beta = entry.eval; break;
            }

            if(alpha >= beta) return entry.eval;
        }
    }

    if(ply == 0) return quiescence(alpha, beta);
    if(_collectStats) ++_stats.nodesSearched;

    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);

    if(moves.empty() && _board.inCheck()) return _nInf;
    if(moves.empty() && !_board.inCheck()) return 0;
    
    int bestEval{_nInf};
    chess::Move bestMove{};
    scoreMoves(moves, false);

    //Main loop
    for(int i = 0; i < moves.size(); i++) {

        if(_stopSearching) return bestEval;

        //Get ordered move
        sortMoves(moves, i);
        const chess::Move& move = moves[i]; 
        
        int curEval;

        //NEGASCOUT: FIRST MOVE
        if(i == 0) {
            //Get eval of move
            _board.makeMove(move);
            curEval = -negamax(ply - 1, -beta, -alpha);
            _board.unmakeMove(move);
        } else {

            _board.makeMove(move);
            curEval = -negamax(ply - 1, -alpha - 1, -alpha);

            if(alpha < curEval && curEval < beta) {
                curEval = -negamax(ply - 1, -beta, -alpha);
            }

            _board.unmakeMove(move);

        }

        //Update Evals
        if(curEval > alpha) alpha = curEval;
        if(curEval > bestEval) {
            bestEval = curEval;
            bestMove = move;
        }

        //Prune
        if(alpha >= beta) {
            if(_collectStats) ++_stats.numCutoffs;
            break;
        }
    }

    //Store data in table
    TTable::Entry entry{};
    if(bestEval <= oldAlpha)  entry.flag = TTable::Entry::UPPER;
    else if(bestEval >= beta) entry.flag = TTable::Entry::LOWER;
    else                      entry.flag = TTable::Entry::EXACT;
    entry.zobrist = _board.hash();
    entry.depth = ply;
    entry.eval = bestEval;
    entry.bestMove = bestMove;
    _table.storeEntry(entry);

    return bestEval;
}

int Engine::quiescence(int alpha, int beta) {
    
    if(_collectStats) ++_stats.quiescenceNodes;

    //Set lower bound
    int sideToMove{_board.sideToMove() == chess::Color::WHITE ? 1 : -1};
    int eval = Evaluation::staticEval(_board) * sideToMove;
    if(eval >= beta) return eval;
    if(eval > alpha) alpha = eval;

    //Generate Capture moves
    chess::Movelist moves{};
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, _board);

    //Setup for loop
    scoreMoves(moves, true);

    for(int i = 0; i < moves.size(); i++) {

        if(_stopSearching) return alpha;

        //Get ordered move
        sortMoves(moves, i);
        const chess::Move& move = moves[i]; 

        //Search move
        _board.makeMove(move);
        int curEval = -quiescence(-beta, -alpha);
        _board.unmakeMove(move);

        //Update Evals
        if(curEval > eval) eval = curEval;

        //Prune
        if(eval > alpha) alpha = eval;
        if(alpha >= beta) break;
    }

    return eval;
}

void Engine::logStatsToFile() {
    std::ofstream logFile(_logFileName, std::ios::app | std::ios::out);

    std::string decimal = std::to_string(_stats.time % 1000);
    while(decimal.length() < 3) decimal = "0" + decimal;
    std::string duration = std::to_string(_stats.time / 1000) + "." + decimal;
    
    logFile << "FEN:" << _board.getFen() << "\n";
    logFile << "\tMOVE:" << chess::uci::moveToSan(_board, _bestMove) <<  " EVAL:" ;
    if(_stats.forcedMate) logFile << "M";
    logFile << _stats.eval << "\n";
    logFile << "\tTIME:" << duration << " DEPTH:" << _stats.depthSearched << "\n";
    logFile << "\tNODES:" << _stats.nodesSearched << " QNODES:" << _stats.quiescenceNodes << "\n";
    logFile << "\tTABLE HITS:" << _stats.tableHits << " CUTOFFS:" << _stats.numCutoffs << "\n";

    logFile << "\n";
    logFile.close();

}

void Engine::scoreMoves(chess::Movelist& moves, bool quiescent) {

    bool tableMoveExists = false;
    chess::Move tableMove;
    if(!quiescent && _table.hasEntry(_board.hash())) {
        tableMove = _table.getEntry(_board.hash()).bestMove;
        tableMoveExists = true;
    }

    for(chess::Move& move: moves) {
        
        if (tableMoveExists && (move == tableMove)) {
            move.setScore(100);
            continue;
        }
        
        if(_board.isCapture(move)) {
            //Ranges from 20 to 30
            chess::PieceType attacker = _board.at<chess::PieceType>(move.from());
            chess::PieceType victim = _board.at<chess::PieceType>(move.to());
            move.setScore(25 + ((int)victim - (int)attacker));
        } else {
            move.setScore(0);
        }
    }
}

void Engine::sortMoves(chess::Movelist& moves, int ind) {

    //Get best move in range
    int maxInd = ind;
    for(int i = ind; i < moves.size(); i++) {
        if(moves[i].score() > moves[maxInd].score()) maxInd = i;
    }

    if(maxInd == ind) return;

    //Swap best move to be first
    chess::Move temp = moves[ind];
    moves[ind] = moves[maxInd];
    moves[maxInd] = temp;
}