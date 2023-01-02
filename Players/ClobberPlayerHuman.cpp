#include "ClobberPlayerHuman.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>

using namespace std;

ClobberPlayerHuman::ClobberPlayerHuman(int color) {
  this->color = color;
  this->moves.clear();
}

int ClobberPlayerHuman::SelectMove(Clobber* c) {
  string player_names = "BW";
  c->GetMoves(this->moves, this->color);
  if (this->moves.empty())
    return -1;
  string move_str;
  int move;
  bool valid_move = false;
  cout << player_names[this->color] << " human:\n";
  while (!valid_move) {
    getline(cin, move_str);
    try {move = c->StrToMove(move_str);} catch(invalid_argument) {move = -1;}
    if (std::find(this->moves.begin(), this->moves.end(), move) != this->moves.end())
      valid_move = true;
    if (!valid_move)
      cout << "invalid move\n";
  }
  this->moves.clear();
  return move;
}