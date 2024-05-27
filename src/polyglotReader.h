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

    const std::string promotions[5] = {"", "k", "b", "r", "q"};

    std::string _fileName{};
    int _numEntries{};
    PolyglotEntry* _fileData{};

    int calcNumEntries();
    PolyglotEntry* readDataFromFile();
    static uint64_t getPolyglotHash(const chess::Board& board);

};