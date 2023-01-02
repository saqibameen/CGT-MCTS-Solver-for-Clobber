#ifndef CLOBBERPLAYERRANDOM_H
#define CLOBBERPLAYERRANDOM_H

#include "ClobberPlayer.h"
#include "../Clobber.h"
#include <random>

using namespace std;

class ClobberPlayerRandom: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
    mt19937 rng;
    int RandInt(int lb, int ub);
  public:
    ClobberPlayerRandom(int color);
    ClobberPlayerRandom(int seed, int color);
    int SelectMove(Clobber* c);
};

#endif