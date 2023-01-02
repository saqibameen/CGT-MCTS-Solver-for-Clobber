#ifndef ABSEARCH_H
#define ABSEARCH_H

#include "TranspositionTable.h"
#include "../../Clobber.h"
#include "../../ZobristHash.h"
#include <vector>
#include <iostream>
#include <stack>

typedef long long int lli;
class ABsearch {
private:
    ZobristHash zh = ZobristHash();
    TranspositionTable* tp;
    lli nodes = -1;
    lli retrieved = 0;
    int first_move;
public:
    ABsearch(Clobber* c);
    ABsearch(Clobber* c, int tablesize, int seed);
    ~ABsearch();

    int NegaScout(Clobber* c, int depth, int player, int tp_table=1, int move_order=0);
    void OrderMoves(vector<int>& moves);

    lli GetNodes();
    lli GetRetrieved();
    lli GetStored();
    lli GetCollisions();
    lli GetReplaced();
    int GetFirstMove();

    void PrintSearchStats();
    void PrintBestMoves(Clobber *c, int player);
};


#endif //ABSEARCH_H
