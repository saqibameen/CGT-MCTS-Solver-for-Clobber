#include "ClobberPlayerMCTS.h"
#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

int ClobberPlayerMCTS::RandInt(int lb, int ub) {
  uniform_int_distribution<int> dist(lb, ub);
  return dist(this->rng);
}

void ClobberPlayerMCTS::InitTable() {
  this->htable = new unsigned int*[MCTS_HASH_CAPACITY]();
  for (int i = 0; i < MCTS_HASH_CAPACITY; i++)
    this->htable[i] = new unsigned int[2]();
}

void ClobberPlayerMCTS::ClearTable() {
  for (int i = 0; i < MCTS_HASH_CAPACITY; i++)
    for (int j = 0; j < 2; j++)
      this->htable[i][j] = 0;
}

ClobberPlayerMCTS::ClobberPlayerMCTS(int color, float t_turn) {
  random_device rd;
  this->rng = mt19937(rd());
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
}

ClobberPlayerMCTS::ClobberPlayerMCTS(int seed, int color, float t_turn) {
  this->rng = mt19937(seed);
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
}

int ClobberPlayerMCTS::SelectMove(Clobber* c) {
  time_t t1 = time(0);
  c->GetMoves(this->moves, this->color);

  if (this->moves.empty()) // player lost
    return -1;
  
  if (this->moves.size() == 1) { // no need to search with 1 move left
    int move = this->moves[0];
    this->moves.clear();
    return move;
  }

  MCTSNode* node;
  this->tree = new MCTSNode(NULL, new Clobber(*c), -1, this->color, this->htable, ZobristHash::ComputeHash(c)); // make tree

  // do MCTS for the alotted time
  time_t duration;
  for (int i = 0; i < MCTS_MAX_SELECTIONS; i++) {
    duration = (float)(time(0) - t1);
    if(duration < this->t_turn) {
      node = this->tree->Selection();
      for (int j = 0; j < MCTS_N_SIMS; j++)
        this->Simulation(node, !node->player);
    } else {
      break;
    }
  }
  
  // pick the best move
  int move = 0;
  float best_val = -1, val;
  for (int i = 0; i < this->tree->sp.size(); i++) {
    val = this->tree->sp[i]->Value();
    if (val > best_val) {
      best_val = val;
      move = this->tree->moves[this->tree->sp[i]->move_i];
    }
  }
  
  cout << "v: " << best_val << ", n: " << this->htable[this->tree->key % MCTS_HASH_CAPACITY][1] << '\n';
  this->moves.clear();
  delete this->tree;
  this->ClearTable();
  return move;
}

void ClobberPlayerMCTS::Simulation(MCTSNode* node, int player) {
  /*
    Two CGT based modifications.
      1. Playout and selection phase of MCTS can be improved by terminating early if a position
        is solved before the game is played out completely.
      2. CGT is used to simplify the boards that MCTS uses to evaluate the moves.
        During selection and playout phase, any two subgames that cancel each other or any 0 game
        can be ignored.
  */
  Clobber* c = new Clobber(*(node->s));
  int turn = player;
  vector<int> moves = node->moves;
  int move;
  while (true) {
    if (moves.empty())
      break;
    move = this->RandInt(0, moves.size() - 1);
    c->Move(moves[move]); // play the move (method from Clobber)
    c->UpdateMoves(moves, moves[move]);
    turn = !turn;
  }
  delete c;
  node->Backprop(player == turn, !player);
  return;
}