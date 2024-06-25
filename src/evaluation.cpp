#include "evaluation.h"
#include <iostream>

int Evaluation::staticEval(chess::Board& board) {
    int eval{0};

    eval += mgPieceValue(board);
    eval += mgPieceTable(board);

    return eval;
}

int Evaluation::mgPieceValue(chess::Board& board) {
    int eval{0};

    for(chess::PieceType piece: _pieces) {
        chess::Bitboard whitePieces = board.pieces(piece, chess::Color::WHITE);
        chess::Bitboard blackPieces = board.pieces(piece, chess::Color::BLACK);
        int pieceValue = _mgPieceValues[static_cast<int>(piece)];
        
        eval += pieceValue * (whitePieces.count() - blackPieces.count());
    }

    return eval;
}

int Evaluation::mgPieceTable(chess::Board& board) {
    int eval{0};
    
    for(chess::PieceType piece: _pieces) {
        chess::Bitboard whitePieces = board.pieces(piece, chess::Color::WHITE);
        chess::Bitboard blackPieces = board.pieces(piece, chess::Color::BLACK);
        int pieceValue{_mgPieceValues[static_cast<int>(piece)]};
        
        while(!whitePieces.empty()) {
            eval += _mgPieceSquareTable[piece][whitePieces.pop()];
        }
        while(!blackPieces.empty()) {
            eval -= _mgPieceSquareTable[piece][63 - blackPieces.pop()];
        }
    }

    return eval;
}