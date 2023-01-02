#ifndef INFORMEDABSEARCH_H
#define INFORMEDABSEARCH_H

#include "../Search/TranspositionTable.h"
#include "../../Clobber.h"
#include "../../ZobristHash.h"
#include "../../Thermographs/ASvalue.h"
#include "../../Thermographs/Database.h"
#include "../../Thermographs/DatabaseEntry.h"
#include "../../Thermographs/InfGamesDefinitions.h"
#include "../../Thermographs/InfinitesimalGame.h"
#include <vector>
#include <iostream>
#include <string>

#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <stack>


typedef long long int lli;
using namespace std::chrono;

class InformedSearch {
private:
    ZobristHash zh = ZobristHash();
    TranspositionTable * tp;
    ReferenceGames rf;
    Database * db;
    lli nodes = -1;
    lli retrieved = 0;
    lli accessed = 0;
    lli max_depth =0;
    int first_move;

//    int attempted =0;
//    lli masks_duration=0;
//    lli db_duration=0;
public:
    InformedSearch(Clobber* c, Database* infdata);
    InformedSearch(Clobber* c, Database* infdata, int tablesize, int seed);
    ~InformedSearch();

    int InformedBoolSearch(Clobber* c, int depth, int player, int tp_table=1, int move_order=0);
    int SearchValue(Clobber* g, vector<__int128> sgmasks, int player);
    int EvaluateGameSum(ASvalue gsum, int player, int exact);
    int EvaluateBoundary(ASvalue boundary, int player);
    void OrderMoves(vector<int>& moves);
    void EliminateIrrelevantGames(Clobber* c, vector<__int128> sgmasks);

    lli GetNodes();
    lli GetAccessed();
    lli GetRetrieved();
    lli GetStored();
    lli GetCollisions();
    lli GetReplaced();
    int GetFirstMove();
//    float GetMaskTime();
//    float GetDbTime();

    void PrintSearchStats();
    void PrintBestMoves(Clobber *c, int player);
};



#endif //INFORMEDABSEARCH_H
