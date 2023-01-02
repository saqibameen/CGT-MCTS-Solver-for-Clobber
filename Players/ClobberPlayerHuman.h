#ifndef CLOBBERPLAYERHUMAN_H
#define CLOBBERPLAYERHUMAN_H

#include "ClobberPlayer.h"
#include "../Clobber.h"

using namespace std;

class ClobberPlayerHuman: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
  public:
    ClobberPlayerHuman(int color);
    int SelectMove(Clobber* c);
};

#endif