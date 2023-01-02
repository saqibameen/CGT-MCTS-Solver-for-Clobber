#include "ClobberPlayerRandom.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

int ClobberPlayerRandom::RandInt(int lb, int ub) {
  uniform_int_distribution<int> dist(lb, ub);
  return dist(this->rng);
}

ClobberPlayerRandom::ClobberPlayerRandom(int color) {
  random_device rd;
  this->rng = mt19937(rd());
  this->color = color;
  this->moves.clear();
}

ClobberPlayerRandom::ClobberPlayerRandom(int seed, int color) {
  this->rng = mt19937(seed);
  this->color = color;
  this->moves.clear();
}

int ClobberPlayerRandom::SelectMove(Clobber* c) {
  c->GetMoves(this->moves, this->color);
  if (this->moves.empty())
    return -1;
  int move = this->moves[this->RandInt(0, this->moves.size() - 1)];
  this->moves.clear();
  return move;
}