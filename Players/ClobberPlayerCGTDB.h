#ifndef CLOBBERPLAYERCGTDB_H
#define CLOBBERPLAYERCGTDB_H

#include "../Clobber.h"
#include "ClobberPlayer.h"
#include "../ZobristHash.h"
#include "../ABSolver.h"
#include "../Thermographs/Database.h"
#include "../Thermographs/DatabaseEntry.h"
#include "../Thermographs/InfGamesDefinitions.h"
#include "../Thermographs/InfinitesimalGame.h"

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
#define CGT_USE_DB true



using namespace std;

class CGTDBNode {
  public:
    CGTDBNode* parent;
    Clobber* s;
    int move_i;
    vector<int> moves;
    vector<CGTDBNode*> sp;
    int player;
    bool expanded;
    unsigned long long int key;
    unsigned int** htable;
    CGTDBNode(CGTDBNode* parent, Clobber* c, int move_i, int player, unsigned int** htable, unsigned long long int key) {
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
    ~CGTDBNode() {
      delete this->s;
      for (int i = 0; i < this->sp.size(); i++)
        delete sp[i];
    }
    CGTDBNode* Selection() {
      // based on the inf, -inf values, skip or not (check thesis)
      CGTDBNode* curr = this;
      CGTDBNode* best = curr;
      float max_UCB1 = -1, sp_UCB1;
      float inf = std::numeric_limits<float>::infinity();

      while (!curr->sp.empty()) { // maximize UCB1 until leaf node
        max_UCB1 = -1;
        for (int i = 0; i < curr->sp.size(); i++) {
          // if node is proven loss, do not consider.

          // Check this again
          if((curr->sp[i]->htable[curr->sp[i]->key % CGT_HASH_CAPACITY][0]) == -inf){
            cout << "Not searching -inf node" << endl;
            continue;
          }

          // if((curr->sp[i]->htable[curr->sp[i]->key % CGT_HASH_CAPACITY][0]) == inf){
          //   cout << "Getting inf node in selection" << endl;
          // }

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
    void Backprop(float val) {
      this->htable[this->key % CGT_HASH_CAPACITY][0] += val;
      this->htable[this->key % CGT_HASH_CAPACITY][1]++;
      if (this->parent != NULL)
        this->parent->Backprop(!val);
    }

    void BackpropProvenWin(float val){

      // if (this->player == player)
      this->htable[this->key % CGT_HASH_CAPACITY][0] = val;
      this->htable[this->key % CGT_HASH_CAPACITY][1]++; // node visit count at index 1.

      /*
      The parent can only be marked as a proven loss if all the other children are also
      proven wins, otherwise in the parent node the player can always choose a move 
      that is not a proven loss for him
      */
     
      if (this->parent != NULL) {
        CGTDBNode* parent = this->parent; // get the current node and check its children
        float inf = std::numeric_limits<float>::infinity();
        bool flag = true;
        int i;
        for (i = 0; i < parent->sp.size(); i++) {
        // all the children are proven wins
        
          // cout << "parent->sp[i]->htable[parent->sp[i]->key % CGT_HASH_CAPACITY][0] " << parent->sp[i]->htable[parent->sp[i]->key % CGT_HASH_CAPACITY][0] << endl;
          if((parent->sp[i]->htable[parent->sp[i]->key % CGT_HASH_CAPACITY][0]) != -inf){
            flag = false;
            break;
          }
        }

        // cout << "How many children are infinity: " << i << endl;
        // exit(0);
        // Confusion
        if (flag){ // if all children are proven wins.

          // cout << "PROVEN WIN" << endl;
          // parent->htable[parent->key % CGT_HASH_CAPACITY][0] = val;
            this->parent->BackpropProvenLoss(-val);
            // this->parent->BackpropProvenLoss(-val, !player);
        } 
        else{
            // run simulations
            this->parent->Backprop(1);
            // this->parent->Backprop(0, !player);
        }
      }

    }

    void BackpropProvenLoss(float val){

      /*
          If a proven loss is backpropagated to a parent, this parent is labeled with a 
          proven win, since in the parent node the player can always select the child 
          with a proven loss for the opponent and win the game.
      */

      // cout << "PROVEN Loss" << endl;

        // cout << "Setting Value" << endl;
      this->htable[this->key % CGT_HASH_CAPACITY][0] = val; 
      this->htable[this->key % CGT_HASH_CAPACITY][1]++; // increase visit count.
      if (this->parent != NULL)
        this->parent->BackpropProvenWin(-val);
        // this->parent->BackpropProvenWin(-val, !player);
    }

    void Expand() {
      unsigned long long int new_key;
      random_shuffle(this->moves.begin(), this->moves.end());
      for (int i = 0; i < moves.size(); i++) {
        new_key = ZobristHash::ComputeHash(this->key, this->player, Clobber::MTFrom[moves[i]], Clobber::MTTo[moves[i]]);
        this->sp.push_back(new CGTDBNode(this, this->s->NextBoard(this->moves[i]), i, !this->player, this->htable, new_key));
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

    float SecureChildValue(){
      /*
        secure child
        Formula => v + A/sqrt(n)
      */
      float n = this->htable[this->key % CGT_HASH_CAPACITY][1];
      if (n == 0) {
        return INT32_MAX;
      }else{
        float v = this->htable[this->key % CGT_HASH_CAPACITY][0];
        float A = 1;
        return v + A/sqrt(n);
      }
    }
};

class ClobberPlayerCGTDB: public ClobberPlayer {
  private:
    int color;
    vector<int> moves;
    Database * db;
    ReferenceGames rf;
    mt19937 rng;
    int RandInt(int lb, int ub);

    void InitTable();
  public:
    CGTDBNode* tree;
    // long double ** htable;
    unsigned int** htable;
    float t_turn;
    void ClearTable();

    ClobberPlayerCGTDB(int color, float t_turn, Database *infdata);
    ClobberPlayerCGTDB(int seed, int color, float t_turn, Database *infdata);

    int SelectMove(Clobber* c);
    void Simulation(CGTDBNode* node, int player);
    int getBoardValueFromDB(CGTDBNode* node);
    int SearchValue(Clobber *g, vector<__int128> sgmasks, int player);
    int EvaluateGameSum(ASvalue gsum, int player, int exact);
    int EvaluateBoundary(ASvalue boundary, int player);
    int dbResultEvaluation(int boardValue, int playerToPlay, int nextPlayerToPlay);

    int MeasureSubgame(__int128 sgmask);
};

#endif