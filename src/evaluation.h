#pragma once

#include "chess.hpp"

class Evaluation {
public:
    
    //Main evaluation function
    static int staticEval(chess::Board& board);

private:

    //Middle game sub eval functions
    static int mgPieceValue(chess::Board& board);
    static int mgPieceTable(chess::Board& board);
    static int mgPawns(chess::Board& board);
    static int mgMobility(chess::Board& board);

    //Pawn helper functions
    static int pawnConnected(chess::Board& board);
    static int wPawnSupport(const chess::Bitboard& pawns, int index);
    static int bPawnSupport(const chess::Bitboard& pawns, int index);
    static int wPawnPhalanx(const chess::Bitboard& pawns, int index);
    static int bPawnPhalanx(const chess::Bitboard& pawns, int index);
    static int wPawnOpposed(chess::Board& board, int index);
    static int bPawnOpposed(chess::Board& board, int index);

    static int pawnIsolated(const chess::Bitboard& pawns, int index);
    static int pawnDoubled(const chess::Bitboard& pawns, int index);

    
    //Constants
    static constexpr chess::PieceType _pieces[5] {
        chess::PieceType::PAWN,
        chess::PieceType::ROOK,
        chess::PieceType::KNIGHT,
        chess::PieceType::BISHOP,
        chess::PieceType::QUEEN
    };

    //Parameters
    static constexpr int _mgPieceValues[5] {100, 320, 330, 500, 900};
    static constexpr int _mgPieceSquareTable[6][64] = {

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
    static constexpr int _mobilityBonus = 10;

    //Pawn Parameters
    static constexpr int _pawnSupporterbonus = 21;
    static constexpr int _pawnConnectedSeed[7] = {0, 7, 8, 12, 29, 48, 86};
    static constexpr int _isolatedPawnPenalty = 5;
    static constexpr int _doubledPawnPenalty = 11;

};