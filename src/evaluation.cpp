#include "evaluation.h"
#include <iostream>

int Evaluation::staticEval(chess::Board& board) {
    int eval = 0;

    eval += mgPieceValue(board);
    eval += mgPieceTable(board);
    eval += mgPawns(board);

    return eval;
}

int Evaluation::mgPieceValue(chess::Board& board) {
    int eval = 0;

    for(chess::PieceType piece: _pieces) {
        chess::Bitboard whitePieces = board.pieces(piece, chess::Color::WHITE);
        chess::Bitboard blackPieces = board.pieces(piece, chess::Color::BLACK);
        int pieceValue = _mgPieceValues[static_cast<int>(piece)];
        
        eval += pieceValue * (whitePieces.count() - blackPieces.count());
    }

    return eval;
}

int Evaluation::mgPieceTable(chess::Board& board) {
    int eval = 0;
    
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

int Evaluation::mgPawns(chess::Board& board) {
    int eval = 0;

    eval += pawnConnected(board);

    return eval;
}

//Returns total score for both sides of connected pawns
int Evaluation::pawnConnected(chess::Board& board) {
    const chess::Bitboard wPawnFull = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    const chess::Bitboard bPawnFull = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    //Supporter Eval
    chess::Bitboard wPawnIter = wPawnFull;
    chess::Bitboard bPawnIter = bPawnFull;

    int whiteSupport = 0;
    int blackSupport = 0;
    int attackTerm = 0;

    while(!wPawnIter.empty()) {
        int ind = wPawnIter.pop();
        whiteSupport += wPawnSupport(wPawnFull, ind);
        
        //Update attack term
        if(ind % 8 == 7) continue;
        int op = wPawnOpposed(board, ind);
        int ph = wPawnPhalanx(wPawnFull, ind);
        int depthBonus = _pawnConnectedSeed[(ind % 8)];
        attackTerm += depthBonus * (2 + ph - op);
    }

    while(!bPawnIter.empty()) {
        int ind = bPawnIter.pop();
        blackSupport -= bPawnSupport(bPawnFull, ind);

        //Update attack term
        if(ind % 8 == 0) continue;
        int op = bPawnOpposed(board, ind);
        int ph = bPawnPhalanx(bPawnFull, ind);
        int depthBonus = _pawnConnectedSeed[7 - (ind % 8)];
        attackTerm -= depthBonus * (2 + ph - op);
    }

    //Calculate final evals
    int supportEval = _pawnSupporterbonus * (whiteSupport - blackSupport);

    return supportEval + attackTerm;
}

//Takes in bitboard of white pawns
int Evaluation::wPawnSupport(const chess::Bitboard& pawns, int index) {
    //Early break for beginning pawns
    if(index < 16) return 0;

    //Detect if pawn on left edge 
    if(index % 8 == 0) {
        //Only check bottom right
        return pawns.check(index - 7);
    }
    //Detect if pawn on right edge
    if(index % 8 == 7) {
        //Only check bottom left
        return pawns.check(index - 9);
    }

    //Check both bottom left and bottom right
    return pawns.check(index - 7) + pawns.check(index - 9);
}

//Takes in bitboard of black pawns
int Evaluation::bPawnSupport(const chess::Bitboard& pawns, int index) {
    //Early break for beginning pawns
    if(index > 40) return 0;

    //Detect if pawn on left edge 
    if(index % 8 == 0) {
        //Only check top right
        return pawns.check(index + 9);
    }
    //Detect if pawn on right edge
    if(index % 8 == 7) {
        //Only check top left
        return pawns.check(index + 7);
    }

    //Check both bottom left and bottom right
    return pawns.check(index + 7) + pawns.check(index + 9);
}

int Evaluation::wPawnPhalanx(const chess::Bitboard& pawns, int index) {
    //Detect if pawn on left edge 
    if(index % 8 == 0) {
        //Only check right
        return pawns.check(index + 1);
    }
    //Detect if pawn on right edge
    if(index % 8 == 7) {
        //Only check left
        return pawns.check(index - 1);
    }

    //Check both left and right
    return pawns.check(index + 1) || pawns.check(index - 1);
}

int Evaluation::bPawnPhalanx(const chess::Bitboard& pawns, int index) {
    //Detect if pawn on left edge 
    if(index % 8 == 0) {
        //Only check right
        return pawns.check(index + 1);
    }
    //Detect if pawn on right edge
    if(index % 8 == 7) {
        //Only check left
        return pawns.check(index - 1);
    }

    //Check both left and right
    return pawns.check(index + 1) || pawns.check(index - 1);
}

int Evaluation::wPawnOpposed(chess::Board& board, int index) {
    //Mask covers entire future path
    chess::Bitboard opposedMask = chess::Bitboard(chess::File(0)) << index;
    chess::Bitboard enemyPawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    //Checks for overlap in mask
    return !((opposedMask & enemyPawns).empty());
}

int Evaluation::bPawnOpposed(chess::Board& board, int index) {
    //Start with mask of just current pawn file
    chess::Bitboard opposedMask = chess::Bitboard(chess::File(index % 8));
    //Shift mask down to pawn
    opposedMask = opposedMask >> (8 * (7 - (index / 8)));

    chess::Bitboard enemyPawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);

    //Checks for overlap in mask
    return !((opposedMask & enemyPawns).empty());
}