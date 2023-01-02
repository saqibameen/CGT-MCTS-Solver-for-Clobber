#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "../../Clobber.h"
#include "../../ZobristHash.h"
#include "HashEntry.h"
#include <iostream>
#include <vector>
#include <cstdlib>

typedef unsigned long long int ulli;
typedef long long int lli;
using namespace std;
const int DEFAULT_HASHSIZE = 15485863;//1280023;//236887691; // //

class TranspositionTable {
private:
    HashEntry* TransTable;
    int size;
public:
    TranspositionTable();
    TranspositionTable(int size);
    ~TranspositionTable();
    void PrintTableStats();

    bool LookUpKey(ulli key, int player);
    void KeepBestKey(ulli key, int depth, int value, int bestMove, int player);
    int RetrieveValue(ulli key, int player);
    void RecordHashEntry(ulli key, int depth, int value, int bestMove, int player);
    void PrintHashEntry(ulli key);

    lli collisions=0;
    lli stored=0;
    lli replaced=0;
};

#endif //TRANSPOSITIONTABLE_H