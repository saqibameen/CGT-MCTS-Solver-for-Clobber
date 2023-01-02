#ifndef CLOBBERPLAYERMCTS_H
#define CLOBBERPLAYERMCTS_H

#include "../Clobber.h"
#include "../ZobristHash.h"
#include "ClobberPlayer.h"

#include <random>
#include <algorithm>
#include <math.h>
#include <cstdint>
#include <iostream>
#include <time.h>

#define MCTS_HASH_CAPACITY 15485863
#define MCTS_MAX_SELECTIONS 5000000
#define MCTS_N_SIMS 5
#define MCTS_UCB1_C 1.4142

using namespace std;

class MCTSNode {
  public:
    MCTSNode* parent;
    Clobber* s;
    int move_i;
    vector<int> moves;
    vector<MCTSNode*> sp;
    int player;
    bool expanded;
    unsigned long long int key;
    unsigned int** htable;
    MCTSNode(MCTSNode* parent, Clobber* c, int move_i, int player, unsigned int** htable, unsigned long long int key) {
      this->s = c;
      this->parent = parent;
      this->sp.reserve(c->Rows() * c->Cols());
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
    }
    ~MCTSNode() {
      delete this->s;
      this->moves.clear();
      for (int i = 0; i < this->sp.size(); i++)
        delete sp[i];
    }
    MCTSNode* Selection() {
      MCTSNode* curr = this;
      MCTSNode* best = curr;
      float max_UCB1 = -1, sp_UCB1;
      while (!curr->sp.empty()) { // maximize UCB1 until leaf node
        max_UCB1 = -1;
        for (int i = 0; i < curr->sp.size(); i++) {
          sp_UCB1 = curr->sp[i]->UCB1();
          if (sp_UCB1 > max_UCB1) {
            max_UCB1 = sp_UCB1;
            best = curr->sp[i];
            if (max_UCB1 == INT32_MAX)
              break;
          }
        }
        curr = best;
      }
      if (max_UCB1 == INT32_MAX) { // select if unvisited leaf
        return curr;
      } else {
        if (!curr->expanded) {
          curr->Expand();
          return curr->Selection(); // go back through selection to make use of hashtable
        }
        if (!curr->sp.empty())
          return curr->sp[0]; // pick child of expanded node
        else
          return curr; // terminal state
      }
    }
    void Backprop(float val, int player) {
      if (this->player == player)
        this->htable[this->key % MCTS_HASH_CAPACITY][0] += val;
      this->htable[this->key % MCTS_HASH_CAPACITY][1]++;
      if (this->parent != NULL)
        this->parent->Backprop(val, player);
    }
    void Expand() {
      unsigned long long int new_key;
      random_shuffle(this->moves.begin(), this->moves.end());
      for (int i = 0; i < this->moves.size(); i++) {
        new_key = ZobristHash::ComputeHash(this->key, this->player, Clobber::MTFrom[moves[i]], Clobber::MTTo[moves[i]]);
        this->sp.push_back(new MCTSNode(this, this->s->NextBoard(this->moves[i]), i, !this->player, this->htable, new_key));
      }
      this->expanded = true;
    }
    float UCB1() {
      float n = this->htable[this->key % MCTS_HASH_CAPACITY][1];
      float N = this->htable[this->parent->key % MCTS_HASH_CAPACITY][1];
      if (n == 0 || N == 0) {
        return INT32_MAX;
      } else {
        float v = this->htable[this->key % MCTS_HASH_CAPACITY][0];
        return (v / n) + MCTS_UCB1_C * sqrt(log(N) / n);
      }
    }
    float Value() {
      float n = this->htable[this->key % MCTS_HASH_CAPACITY][1];
      if (n == 0) {
        return INT32_MAX;
      } else {
        float v = this->htable[this->key % MCTS_HASH_CAPACITY][0];
        return v / n;
      }
    }
};

class ClobberPlayerMCTS: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
    
    mt19937 rng;
    int RandInt(int lb, int ub);

    void InitTable();
  public:
    MCTSNode* tree;
    unsigned int** htable;
    float t_turn;

    void ClearTable();

    ClobberPlayerMCTS(int color, float t_turn);
    ClobberPlayerMCTS(int seed, int color, float t_turn);

    int SelectMove(Clobber* c);
    void Simulation(MCTSNode* node, int player);
};

#endif