#pragma once

#include "chess.hpp"

class TTable {
public:
    struct Entry {

        enum {
            EXACT,
            LOWER,
            UPPER
        } flag;

        uint64_t zobrist{};
        int eval{};
        int depth{};
        chess::Move bestMove{};
    };

    TTable(int size);
    ~TTable();

    bool hasEntry(uint64_t zobrist);
    const Entry& getEntry(uint64_t zobrist);

    void storeEntry(Entry entry);
    void resize(int size);

    void clear();

private:

    Entry* _table{};
    int _numEntries{0};
    int _size{0};
    
};