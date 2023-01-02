#ifndef CLOBBERPLAYER_H
#define CLOBBERPLAYER_H

#include "../Clobber.h"

using namespace std;

class ClobberPlayer {
  public:
    virtual ~ClobberPlayer() {};
    virtual void ClearTable() {};
    virtual int SelectMove(Clobber* c) = 0;
};

#endif