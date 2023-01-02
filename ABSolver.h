// Adapted from Fernando's implementation

#ifndef ABSOLVER_H
#define ABSOLVER_H

#include "Clobber.h"
#include "ZobristHash.h"
#include <vector>
#include <iostream>

#define TT_CAPACITY 1280023

class TTEntry {
  public:
    unsigned long long int key;
    int depth;
    int val;
    TTEntry() {
      this->key = this->depth = this->val = 0;
    }
    void Set(unsigned long long int key, int depth, int val) {
      this->key = key;
      this->depth = depth;
      this->val = val;
    }
};

class ABSolver {
  private:
    static TTEntry** tt;
  public:
    static void InitTT();
    static void ClearTT();
    static void DeleteTT();
    static bool InTT(unsigned long long int key, int player);
    static int GetTTVal(unsigned long long int key, int player);
    static void UpdateTT(unsigned long long int key, int depth, int val, int player);
    static int BinSearch(Clobber* c, unsigned long long int key, vector<int>& move_list, int depth, int player);
    static int BinSumSearch(vector<Clobber*>& c, unsigned long long int key, vector<vector<int> >& move_lists, int depth, int player);
    static int OutcomeClass(Clobber *c, int max_depth);
    static int SimpleInf(Clobber *c, int max_depth);
    static bool IsZero(Clobber *c, int max_depth);
    static bool IsStar(Clobber *c, int max_depth);
    static bool IsUp(Clobber *c, int max_depth);
    static bool IsDown(Clobber *c, int max_depth);
    static bool IsUpStar(Clobber *c, int max_depth);
    static bool IsDownStar(Clobber *c, int max_depth);
    static bool IsUp2Star(Clobber *c, int max_depth);
    static bool IsDown2Star(Clobber *c, int max_depth);
    static bool IsUpStarSwitch(Clobber *c, int max_depth);
};

#endif //ABSOLVER_H
