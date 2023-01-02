#ifndef CLOBBERPLAYERCGT_H
#define CLOBBERPLAYERCGT_H

#include "../Clobber.h"
#include "ClobberPlayer.h"
#include "../ZobristHash.h"
#include "../ABSolver.h"

#include <random>
#include <algorithm>
#include <math.h>
#include <cstdint>
#include <iostream>
#include <time.h>

#define CGT_HASH_CAPACITY 15485863
#define CGT_MAX_SELECTIONS 5000000
#define CGT_N_SIMS 5
#define CGT_UCB1_C 1.4142
#define CGT_MAX_SG_SIZE 21

using namespace std;

class CGTNode {
  public:
    CGTNode* parent;
    Clobber* s;
    int move_i;
    vector<int> moves;
    vector<CGTNode*> sp;
    int player;
    bool expanded;
    unsigned long long int key;
    unsigned int** htable;
    CGTNode(CGTNode* parent, Clobber* c, int move_i, int player, unsigned int** htable, unsigned long long int key) {
      this->s = c;
      this->parent = parent;
      this->move_i = move_i;
      this->player = player;
      this->expanded = false;
      this->htable = htable;
      this->key = key;
      if (parent == NULL) {
        this->s->GetMoves(this->moves, this->player);
      } else {
        this->moves = this->parent->moves;
        this->s->UpdateMoves(this->moves, this->moves[move_i]);
      }
      this->sp.reserve(this->moves.size());
    }
    ~CGTNode() {
      delete this->s;
      for (int i = 0; i < this->sp.size(); i++)
        delete sp[i];
    }
    CGTNode* Selection() {
      CGTNode* curr = this;
      CGTNode* best = curr;
      float max_UCB1 = -1, sp_UCB1;
      while (!curr->sp.empty()) { // maximize UCB1 until leaf node
        max_UCB1 = -1;
        for (int i = 0; i < curr->sp.size(); i++) {
          sp_UCB1 = curr->sp[i]->UCB1();
          if (sp_UCB1 > max_UCB1) {
            max_UCB1 = sp_UCB1;
            best = curr->sp[i];
            if (max_UCB1 >= INT32_MAX)
              return best;
          }
        }
        curr = best;
      }
      if (!curr->expanded) {
        curr->Expand();
        return curr->Selection(); // go back through selection to make use of hashtable
      } else {
      if (!curr->sp.empty())
        return curr->sp[0]; // pick child of expanded node
      else
        return curr; // terminal state
      }
    }
    void Backprop(float val, int player) {
      if (this->player == player)
        this->htable[this->key % CGT_HASH_CAPACITY][0] += val;
      this->htable[this->key % CGT_HASH_CAPACITY][1]++;
      if (this->parent != NULL)
        this->parent->Backprop(val, player);
    }
    void Expand() {
      unsigned long long int new_key;
      random_shuffle(this->moves.begin(), this->moves.end());
      for (int i = 0; i < moves.size(); i++) {
        new_key = ZobristHash::ComputeHash(this->key, this->player, Clobber::MTFrom[moves[i]], Clobber::MTTo[moves[i]]);
        this->sp.push_back(new CGTNode(this, this->s->NextBoard(this->moves[i]), i, !this->player, this->htable, new_key));
      }
      this->expanded = true;
    }
    float UCB1() {
      float N = this->htable[this->parent->key % CGT_HASH_CAPACITY][1];
      float n = this->htable[this->key % CGT_HASH_CAPACITY][1];
      if (n == 0 || N == 0) {
        return INT32_MAX;
      } else {
        float v = this->htable[this->key % CGT_HASH_CAPACITY][0];
        return (v / n) + CGT_UCB1_C * sqrt(log(N) / n);
      }
    }
    float Value() {
      float n = this->htable[this->key % CGT_HASH_CAPACITY][1];
      if (n == 0) {
        return INT32_MAX;
      } else {
        float v = this->htable[this->key % CGT_HASH_CAPACITY][0];
        return v / n;
      }
    }
    
};

class ClobberPlayerCGT: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
    
    mt19937 rng;
    int RandInt(int lb, int ub);

    void InitTable();
  public:
    CGTNode* tree;
    unsigned int** htable;
    float t_turn;

    void ClearTable();

    ClobberPlayerCGT(int color, float t_turn);
    ClobberPlayerCGT(int seed, int color, float t_turn);

    int SelectMove(Clobber* c);
    void Simulation(CGTNode* node, int player);

    int MeasureSubgame(__int128 sgmask);
};

#endif