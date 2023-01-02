#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H

#include "Clobber.h"
#include <random>

using namespace std;

class ZobristHash {
  private:
    static const size_t MAX_HASH_INDEX = 256;
    static const size_t NUM_PIECES = 3;
    static unsigned long long int zobrist_table[MAX_HASH_INDEX * NUM_PIECES];

    static unsigned long long int RandInt64(mt19937*);
  public:
    static void InitTable();
    static void InitTable(int seed);
    static unsigned long long int ComputeHash(Clobber* c);
    static unsigned long long int ComputeHash(vector<Clobber*> cs);
    static unsigned long long int ComputeHash(unsigned long long int hash_value, int player, int x1, int x2);
    static unsigned long long int Get(int index, int piece);
};

#endif