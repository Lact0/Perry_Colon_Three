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

void Engine::makeMove(const chess::Move& move) {
    _board.makeMove(move);
    _bestMove = chess::Move();
}

void Engine::think(int maxPly) {

    //Gen moves
    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);

    //Look up book moves
    chess::Movelist bookMoves{};
    if(_useOpeningBook) getBookMoves(bookMoves);
    bool bookMoveExists{bookMoves.size() > 0};

    for(int ply{0}; ply < maxPly; ++ply) {

        int bestEval{_nInf};
        chess::Move bestMove{};

        for(const chess::Move& move: moves) {
            
            _board.makeMove(move);
            int curEval{-negamax(ply)};
            _board.unmakeMove(move);

            if(curEval > bestEval) {
                bestEval = curEval;
                bestMove = move;
            }

        }

        _eval = bestEval;
        _bestMove = bestMove;

    }

    //Bookmove trumps search
    if(bookMoveExists) _bestMove = bookMoves[0];

}

int Engine::negamax(int ply) {

    chess::Movelist moves{};
    chess::movegen::legalmoves(moves, _board);

    if(moves.empty() && _board.inCheck()) return _nInf;
    if(moves.empty() && !_board.inCheck()) return 0;

    int sideToMove{_board.sideToMove() == chess::Color::WHITE ? 1 : -1};
    if(ply == 0) return staticEval() * sideToMove;
    
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
        
        eval += (whitePieces.count() - blackPieces.count()) * pieceValue;
    }

    return eval;
}