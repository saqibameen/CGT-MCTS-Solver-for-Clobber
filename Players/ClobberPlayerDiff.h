#ifndef CLOBBERPLAYERDIFF_H
#define CLOBBERPLAYERDIFF_H

#include "ClobberPlayer.h"
#include "../Clobber.h"
#include <random>

using namespace std;

class ClobberPlayerDiff: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
    mt19937 rng;
    int RandInt(int lb, int ub);
  public:
    ClobberPlayerDiff(int color);
    ClobberPlayerDiff(int seed, int color);
    int SelectMove(Clobber* c);
};

#endif