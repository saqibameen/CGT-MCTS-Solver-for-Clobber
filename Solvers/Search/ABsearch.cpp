//
// Created by jfernando on 18/03/17.
//

#include "ABsearch.h"
using namespace std;

ABsearch::ABsearch(Clobber *c) {
  zh = ZobristHash();
  if (zh.RetrieveHash() == 0)
    zh.StoreHash(c);
  tp = new TranspositionTable();
}

ABsearch::ABsearch(Clobber *c, int tablesize, int seed) {
  zh = ZobristHash(seed);
  if (zh.RetrieveHash() == 0)
    zh.StoreHash(c);
  tp = new TranspositionTable(tablesize);
}

ABsearch::~ABsearch(){
  delete tp;
}

int ABsearch::NegaScout(Clobber *c, int depth, int player, int tp_table, int move_order) {
  this->nodes += 1;
  vector<int> moves;
  int value = -1;
  int next_player = (player * -1) + 1;
  c->GetMoves(moves, player);
  if (move_order)
    this->OrderMoves(moves);

  // Check if leaf node
  if (moves.empty()) {
    value = next_player; // 1 if the player is black => white wins, 0 if the player is white == black wins
    tp->KeepBestKey(zh.RetrieveHash(), -1, value, 0, player);
    return value;
  }

  // Explore possible moves
  for (int i=0; i < moves.size(); ++i){
    c->Move(moves[i]);
    zh.UpdateHash(player, c->GetFrom(moves[i]), c->GetTo(moves[i]));

    //// Check Transposition Table
    if (tp_table) {
      bool intable = tp->LookUpKey(zh.RetrieveHash(), player);
      if (intable) {
        this->retrieved += 1;
        this->nodes += 1;
        value = tp->RetrieveValue(zh.RetrieveHash(), player);
      }
      else{ /// Keep Searching
        value = this->NegaScout(c, depth-1, next_player, tp_table, move_order);
      }
    }
    else{ /// Kee searching
      value = this->NegaScout(c, depth-1, next_player, tp_table, move_order);
    }

    tp->KeepBestKey(zh.RetrieveHash(), depth, value, moves[i], player);
    zh.UpdateHash(player, c->GetFrom(moves[i]), c->GetTo(moves[i])); // Reverses the hash value
    c->ReverseMove(moves[i]); // Reverses the board

    this->first_move = moves[i];
    if (value == player)  // The player found a winning move
      break;
  }

  return value;
}

void ABsearch::OrderMoves(vector<int> &moves) {
  int move = 0;
  for (int i = 0; i < moves.size() / 2; i++) {
    move = moves.front();
    moves.erase(moves.begin());
    moves.push_back(move);
  }
}

/// Summary
void ABsearch::PrintSearchStats() {
  cout << "Search Statistics..." << endl;
  cout << "Nodes: " << this->nodes << endl;
  cout << "Retrieved: " << this->retrieved << endl;
  cout << "Table Statistics..." << endl;
  tp->PrintTableStats();
}

void ABsearch::PrintBestMoves(Clobber *c, int player) {
  vector<int> moves;
  c->GetMoves(moves, player);
  c->PrintBoard();
  stack<int> moves_stack;
  while (!moves.empty()) {
    ABsearch temp_search = ABsearch(c);
    temp_search.NegaScout(c, 100, player);
    int move = temp_search.GetFirstMove();
    moves_stack.push(move);
    cout << "Move: " << c->MoveToStr(move) << endl;
    c->Move(move);
    c->PrintBoard();
    moves.clear();
    player = !player;
    c->GetMoves(moves, player);
  }
  while (!moves_stack.empty()){
    c->ReverseMove(moves_stack.top());
    moves_stack.pop();
  }
}

/// Getters
lli ABsearch::GetNodes() {
  return this->nodes;
}

lli ABsearch::GetRetrieved() {
  return this->retrieved;
}

lli ABsearch::GetStored() {
  return tp->stored;
}

lli ABsearch::GetCollisions() {
  return tp->collisions;
}

lli ABsearch::GetReplaced() {
  return tp->replaced;
}

int ABsearch::GetFirstMove() {
  return this->first_move;
}
