#pragma once

#include "chess.hpp"
#include <fstream>

/*
TO USE:

    Instantiate an object with .bin file name
    Call .getMoves() with:
        Moves - movelist out parameter
        Board - current board state


*/


class PolyglotReader {
public:

    struct PolyglotEntry {
        uint64_t key{};
        uint16_t move{};
        uint16_t weight{};
        uint32_t learn{};
    };

    PolyglotReader(std::string_view fileName)
        : _numEntries{calcNumEntries()}
        , _fileName{fileName}
        , _fileData{readDataFromFile()}
    {}

    ~PolyglotReader() {
        delete[] _fileData;
    }

    int getNumEntries() {return _numEntries;}

    void getMoves(chess::Movelist& moves, const chess::Board& board);

private:

    const char _promotions[5] = {'\0', 'k', 'b', 'r', 'q'};
    const std::string _castleMoves[4] = {"e1h1", "e1a1", "e8h8", "e8a8"};
    const std::string _correctCastleMoves[4] = {"e1g1", "e1c1", "e8g8", "e8c8"};

    std::string _fileName{};
    int _numEntries{};
    PolyglotEntry* _fileData{};

    int calcNumEntries();
    PolyglotEntry* readDataFromFile();
    static uint64_t getPolyglotHash(const chess::Board& board);

};