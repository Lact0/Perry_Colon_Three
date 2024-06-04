#include "engine.h"

void Engine::setBoard(chess::Board board) {
    _board = board;
    _bestMove = chess::Move();
    _eval = staticEval();
}

void Engine::useOpeningBook(std::string_view fileName) {
    _useOpeningBook = true;
    _openingBook.emplace(fileName);

}

void Engine::finishSearching() {
    _stopSearching = true;
    _thinkThread.join();
}

void Engine::logStats(std::string_view logFileName) {
    _logStats = true;
    _logFileName = logFileName;
}

void Engine::makeMove(const chess::Move& move) {
    _board.makeMove(move);
    _bestMove = chess::Move();
    _eval = staticEval();
}

void Engine::think(int maxPly) {

    //Prevent from starting a second think thread
    if(_isSearching) return;

    //Setup concurrency variables
    _stopSearching = false;
    _isSearching = true;

    //Start thread
    _thinkThread = std::thread{&Engine::thinkWorker, this, maxPly};
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

    //Look up book moves
    chess::Movelist bookMoves{};
    if(_useOpeningBook) getBookMoves(bookMoves);
    bool bookMoveExists{bookMoves.size() > 0};

    //Iterative Deepening
    for(int ply{0}; ply < maxPly; ++ply) {

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

        _eval = bestEval;
        _bestMove = bestMove;

        if(_collectStats) _stats.depthSearched = ply + 1;

    }

    //Bookmove trumps search
    if(bookMoveExists) _bestMove = bookMoves[0];

    if(_collectStats) {
        time end = std::chrono::high_resolution_clock::now();
        _stats.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    if(_logStats) logStatsToFile();

    _isSearching = false;
}

int Engine::negamax(int ply, int alpha, int beta) {

    if(_stopSearching) return 0;
    if(_collectStats) ++_stats.nodesSearched;

    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);

    if(moves.empty() && _board.inCheck()) return _nInf;
    if(moves.empty() && !_board.inCheck()) return 0;
    if(_board.isRepetition(1)) return 0;

    int sideToMove{_board.sideToMove() == chess::Color::WHITE ? 1 : -1};
    if(ply == 0) return staticEval() * sideToMove;
    
    int bestEval{_nInf};

    //Main loop
    for(const chess::Move& move: moves) {

        if(_stopSearching) return bestEval;
        
        //Get eval of move
        _board.makeMove(move);
        int curEval = -negamax(ply - 1, -beta, -alpha);
        _board.unmakeMove(move);

        //Later positions are worth less
        if(curEval > 0) --curEval;
        if(curEval < 0) ++curEval;

        //Update Evals
        if(curEval > alpha) alpha = curEval;
        if(curEval > bestEval) bestEval = curEval;

        //Prune
        if(alpha >= beta) {
            if(_collectStats) ++_stats.numCutoffs;
            break;
        }
    }

    return bestEval;
}

int Engine::staticEval() {
    int eval{0};

    chess::PieceType piecesToScore[5] {
        chess::PieceType::PAWN,
        chess::PieceType::ROOK,
        chess::PieceType::KNIGHT,
        chess::PieceType::BISHOP,
        chess::PieceType::QUEEN
    };

    for(chess::PieceType piece: piecesToScore) {
        chess::Bitboard whitePieces = _board.pieces(piece, chess::Color::WHITE);
        chess::Bitboard blackPieces = _board.pieces(piece, chess::Color::BLACK);
        int pieceValue{_pieceValues[static_cast<int>(piece)]};
        
        while(!whitePieces.empty()) {
            eval += pieceValue + _pieceSquareTable[piece][whitePieces.pop()];
        }
        while(!blackPieces.empty()) {
            eval -= pieceValue + _pieceSquareTable[piece][63 - blackPieces.pop()];
        }
    }

    return eval;
}

void Engine::logStatsToFile() {
    std::ofstream logFile(_logFileName, std::ios::app | std::ios::out);

    std::string duration = std::to_string(_stats.time / 1000) + "." + std::to_string(_stats.time % 1000);
    
    logFile << "FEN:" << _board.getFen() << "\n";
    logFile << "\tMOVE:" << chess::uci::moveToSan(_board, _bestMove) <<  " EVAL:" << _eval << "\n";
    logFile << "\tTIME:" << duration << " DEPTH:" << _stats.depthSearched << "\n";
    logFile << "\tNODES:" << _stats.nodesSearched << " Cutoffs:" << _stats.numCutoffs << "\n";

    logFile << "\n";
    logFile.close();

}