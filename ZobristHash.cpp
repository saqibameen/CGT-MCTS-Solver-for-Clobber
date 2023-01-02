#include "ZobristHash.h"

#include <iostream>
#include <cstdint>

unsigned long long int ZobristHash::zobrist_table[MAX_HASH_INDEX * NUM_PIECES];

unsigned long long int ZobristHash::RandInt64(mt19937* mt) {
  uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
  return dist(*mt);
}

void ZobristHash::InitTable() {
  random_device rd;
  mt19937 mt(rd());
  for (int i = 0; i < MAX_HASH_INDEX * NUM_PIECES; i++)
    zobrist_table[i] = RandInt64(&mt);
}

void ZobristHash::InitTable(int seed) {
  mt19937 mt(seed);
  for (int i = 0; i < MAX_HASH_INDEX * NUM_PIECES; i++)
    zobrist_table[i] = RandInt64(&mt);
}

unsigned long long int ZobristHash::ComputeHash(Clobber* c) {
  unsigned long long int hash = 0;
  for (int i = 0; i < c->Rows(); i++)
    for (int j = 0; j < c->Cols(); j++)
      hash ^= Get(c->Index(i, j), c->GetPiece(i, j));
  return hash;
}

unsigned long long int ZobristHash::ComputeHash(vector<Clobber*> cs) {
  int csize = cs[0]->Rows() * cs[0]->Cols();
  unsigned long long int hash = 0;
  for (int c = 0; c < cs.size(); c++)
    for (int i = 0; i < cs[c]->Rows(); i++)
      for (int j = 0; j < cs[c]->Cols(); j++)
        hash ^= Get(c * csize + cs[c]->Index(i, j), cs[c]->GetPiece(i, j));
  return hash;
}

unsigned long long int ZobristHash::ComputeHash(unsigned long long int hash, int player, int x1, int x2) {
  return hash ^ Get(x1, player) ^ Get(x1, 2) ^ Get(x2, !player) ^ Get(x2, player);
}

unsigned long long int ZobristHash::Get(int index, int piece) {
  return zobrist_table[index * NUM_PIECES + piece];
}