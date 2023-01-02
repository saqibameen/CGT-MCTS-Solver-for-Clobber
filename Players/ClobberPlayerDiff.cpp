#include "ClobberPlayerDiff.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

int ClobberPlayerDiff::RandInt(int lb, int ub) {
  uniform_int_distribution<int> dist(lb, ub);
  return dist(this->rng);
}

ClobberPlayerDiff::ClobberPlayerDiff(int color) {
  random_device rd;
  this->rng = mt19937(rd());
  this->color = color;
  this->moves.clear();
}

ClobberPlayerDiff::ClobberPlayerDiff(int seed, int color) {
  this->rng = mt19937(seed);
  this->color = color;
  this->moves.clear();
}

int ClobberPlayerDiff::SelectMove(Clobber* c) {
  c->GetMoves(this->moves, this->color);
  if (this->moves.empty())
    return -1;
  vector<int> maxmoves;
  maxmoves.reserve(this->moves.size());
  Clobber* c2;
  int diff, max_diff = -(c->Rows() * c->Cols() / 2);
  for (int i = 0; i < this->moves.size(); i++) {
    c2 = c->NextBoard(this->moves[i]);
    diff = c2->ActiveDifferential(this->color);
    if (diff > max_diff) {
      max_diff = diff;
      maxmoves.clear();
      maxmoves.push_back(moves[i]);
    } else if (diff == max_diff) {
      maxmoves.push_back(moves[i]);
    }
    delete c2;
  }
  int move = maxmoves[this->RandInt(0, maxmoves.size() - 1)];
  this->moves.clear();
  return move;
}