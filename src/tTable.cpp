#include "tTable.h"

TTable::TTable(int size) {
    _size = size;
    _numEntries = size * 1000 / sizeof(TTable::Entry);
    _table = new Entry[_numEntries];
}

TTable::~TTable() {
    clear();
}

bool TTable::hasEntry(uint64_t zobrist) {
    if(_numEntries == 0) return false;

    int index = zobrist % _numEntries;
    return _table[index].zobrist == zobrist;
}

const TTable::Entry& TTable::getEntry(uint64_t zobrist) {
    return _table[zobrist % _numEntries];
}

void TTable::storeEntry(TTable::Entry entry) {
    if(_numEntries == 0) return;
    _table[entry.zobrist % _numEntries] = entry;
}

void TTable::clear() {
    _size = 0;
    _numEntries = 0;
    delete[] _table;
}