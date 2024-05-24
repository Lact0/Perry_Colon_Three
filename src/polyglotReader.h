#pragma once

#include "chess.hpp"
#include <fstream>

class PolyglotReader {
public:

    struct PolyglotEntry {
        uint64_t key{};
        uint16_t move{};
        uint16_t weight{};
        uint32_t learn{};
    };

    PolyglotReader(const std::string& fileName)
        : _numEntries{calcNumEntries()}
        , _fileName{fileName}
        , _fileData{readDataFromFile()}
    {}

    ~PolyglotReader() {
        delete[] _fileData;
    }

    void getMoves(chess::Movelist& moves, const chess::Board& board);
    int getNumEntries() {return _numEntries;}

    static uint64_t getPolyglotHash(const chess::Board& board);


private:

    const std::string promotions[5] = {"", "k", "b", "r", "q"};

    std::string _fileName{};
    int _numEntries{};
    PolyglotEntry* _fileData{};

    int calcNumEntries();
    PolyglotEntry* readDataFromFile();

};