#include "ABSolver.h"
using namespace std;

TTEntry** ABSolver::tt;

void ABSolver::InitTT() {
  tt = new TTEntry*[TT_CAPACITY]();
  for (int i = 0; i < TT_CAPACITY; i++)
    tt[i] = new TTEntry[2]();
}

void ABSolver::ClearTT() {
  for (int i = 0; i < TT_CAPACITY; i++)
    for (int j = 0; j < 2; j++)
      tt[i][j].Set(0, 0, 0);
}

void ABSolver::DeleteTT() {
  for (int i = 0; i < TT_CAPACITY; i++)
    delete [] tt[i];
  delete [] tt;
}

bool ABSolver::InTT(unsigned long long int key, int player) {
  return tt[key % TT_CAPACITY][player].key == key;
}

int ABSolver::GetTTVal(unsigned long long int key, int player) {
  return tt[key % TT_CAPACITY][player].val;
}

void ABSolver::UpdateTT(unsigned long long int key, int depth, int val, int player) {
  TTEntry* entry = &tt[key % TT_CAPACITY][player];
  if (entry->key != 0 && entry->key != key) {
    if (depth > entry->depth)
      entry->Set(key, depth, val);
  } else {
    entry->Set(key, depth, val);
  }
}

int ABSolver::BinSearch(Clobber* c, unsigned long long int key, vector<int>& move_list, int depth, int player) {
  unsigned long long int next_key;
  Clobber* next_c;
  vector<int> next_moves;
  int value;
  if (move_list.empty()) {
    value = !player;
    UpdateTT(key, -1, value, player);
    delete c;
    return value;
  }
  if (depth > 0) {
    for (int i = 0; i < move_list.size(); i++) {
      next_key = ZobristHash::ComputeHash(key, player, Clobber::MTFrom[move_list[i]], Clobber::MTTo[move_list[i]]);
      if (InTT(next_key, !player)) {
        value = GetTTVal(next_key, !player);
      } else {
        next_c = c->NextBoard(move_list[i]); next_moves = move_list; next_c->UpdateMoves(next_moves, move_list[i]);
        value = BinSearch(next_c, next_key, next_moves, depth - 1, !player);
      }
      UpdateTT(next_key, depth, value, !player);
      if (value == player || value == -1)
        break;
    }
  } else {
    value = -1;
  }
  delete c;
  return value;
}

int ABSolver::BinSumSearch(vector<Clobber*>& cs, unsigned long long int key, vector<vector<int> >& move_lists, int depth, int player) {
  int csize = cs[0]->Rows() * cs[0]->Cols();
  unsigned long long int next_key;
  vector<Clobber*> next_cs;
  vector<vector<int> > next_moves;
  int value;
  bool no_moves = true;
  for (int i = 0; i < move_lists.size(); i++) {
    if (!move_lists[i].empty()) {
      no_moves = false;
      break;
    }
  }
  if (no_moves) {
    value = !player;
    UpdateTT(key, -1, value, player);
    for (int i = 0; i < cs.size(); i++)
      delete cs[i];
    return value;
  }
  if (depth > 0) {
    for (int i = 0; i < move_lists.size(); i++) {
      for (int j = 0; j < move_lists[i].size(); j++) {
        next_key = ZobristHash::ComputeHash(key, player, i * csize + Clobber::MTFrom[move_lists[i][j]], i * csize + Clobber::MTTo[move_lists[i][j]]);
        if (InTT(next_key, !player)) {
          value = GetTTVal(next_key, !player);
        } else {
          next_cs = cs; next_moves = move_lists;
          for (int k = 0; k < next_cs.size(); k++) {
            if (k == i) {
              next_cs[k] = next_cs[k]->NextBoard(move_lists[k][j]);
              next_cs[k]->UpdateMoves(next_moves[k], move_lists[k][j]);
            } else {
              next_cs[k] = new Clobber(*next_cs[k]);
              next_cs[k]->FlipMoves(next_moves[k]);
            }
          }
          value = BinSumSearch(next_cs, next_key, next_moves, depth - 1, !player);
        }
        UpdateTT(next_key, depth, value, !player);
        if (value == player || value == -1)
          break;
      }
      if (value == player || value == -1)
        break;
    }
  } else {
    value = -1;
  }
  for (int i = 0; i < cs.size(); i++)
    delete cs[i];
  return value;
}

int ABSolver::OutcomeClass(Clobber *c, int max_depth) {
  int b, w;
  unsigned long long int key = ZobristHash::ComputeHash(c);
  vector<int> moves;
  c->GetMoves(moves, 0);
  b = BinSearch(new Clobber(*c), key, moves, max_depth, 0);
  c->FlipMoves(moves);
  w = BinSearch(new Clobber(*c), key, moves, max_depth, 1);
  if (b == -1 || w == -1)
    return -1; // depth limit reached
  if (b == 1 && w == 0)
    return 0; // P position
  if (b == 0 && w == 1)
    return 1; // N position
  if (b == 0 && w == 0)
    return 2; // L position
  if (b == 1 && w == 1)
    return 3; // R position
  return -1;
}

int ABSolver::SimpleInf(Clobber *c, int max_depth) {
  int cols = c->Cols();
  if (IsZero(c, max_depth))
    return 0;
  else if (cols >= 2 && IsStar(c, max_depth))
    return 1;
  else if (cols >= 3 && IsUp(c, max_depth))
    return 2;
  else if (cols >= 3 && IsDown(c, max_depth))
    return 3;
  else if (cols >= 4 && IsUpStar(c, max_depth))
    return 4;
  else if (cols >= 4 && IsDownStar(c, max_depth))
    return 5;
  else if (cols >= 4 && IsUp2Star(c, max_depth))
    return 6;
  else if (cols >= 4 && IsDown2Star(c, max_depth))
    return 7;
  //else if (cols >= 4 && IsUpStarSwitch(c, max_depth))
  //  return 8;
  else
    return -1;
}

bool ABSolver::IsZero(Clobber *c, int max_depth) {
  int b, w;
  unsigned long long int key = ZobristHash::ComputeHash(c);
  vector<int> moves;
  c->GetMoves(moves, 0);
  b = BinSearch(new Clobber(*c), key, moves, max_depth, 0);
  c->FlipMoves(moves);
  w = BinSearch(new Clobber(*c), key, moves, max_depth, 1);
  return (b == 1 && w == 0);
}

// game must have at least 2 cols for the following
bool ABSolver::IsStar(Clobber* c, int max_depth) {
  int b, w;
  Clobber* star = new Clobber(*c); star->ClearBoard();
  star->SetPiece(0, 0); star->SetPiece(1, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*star));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*star));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); star->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); star->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete star;
  return (b == 1 && w == 0);
}

// game must have at least 3 cols for the following
bool ABSolver::IsUp(Clobber* c, int max_depth) {
  int b, w;
  Clobber* down = new Clobber(*c); down->ClearBoard();
  down->SetPiece(0, 0); down->SetPiece(1, 1); down->SetPiece(2, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*down));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*down));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); down->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); down->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete down;
  return (b == 1 && w == 0);
}

bool ABSolver::IsDown(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 0); up->SetPiece(1, 0); up->SetPiece(2, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}

// game must have at least 4 cols for the following
bool ABSolver::IsUpStar(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 0); up->SetPiece(1, 1); up->SetPiece(2, 0); up->SetPiece(3, 0);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}

bool ABSolver::IsDownStar(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 1); up->SetPiece(1, 0); up->SetPiece(2, 1); up->SetPiece(3, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}

bool ABSolver::IsUp2Star(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 0); up->SetPiece(1, 1); up->SetPiece(2, 1); up->SetPiece(3, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}

bool ABSolver::IsDown2Star(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 1); up->SetPiece(1, 0); up->SetPiece(2, 0); up->SetPiece(3, 0);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}

bool ABSolver::IsUpStarSwitch(Clobber* c, int max_depth) {
  int b, w;
  Clobber* up = new Clobber(*c); up->ClearBoard();
  up->SetPiece(0, 0); up->SetPiece(1, 1); up->SetPiece(2, 0); up->SetPiece(3, 1);
  vector<Clobber*> csb, csw;
  csb.push_back(new Clobber(*c)); csb.push_back(new Clobber(*up));
  csw.push_back(new Clobber(*c)); csw.push_back(new Clobber(*up));
  unsigned long long int key = ZobristHash::ComputeHash(csb);
  vector<vector<int> > moves; vector<int> m0, m1;
  moves.push_back(m0); moves.push_back(m1);
  c->GetMoves(moves[0], 0); up->GetMoves(moves[1], 0);
  b = BinSumSearch(csb, key, moves, max_depth, 0);
  c->FlipMoves(moves[0]); up->FlipMoves(moves[1]);
  w = BinSumSearch(csw, key, moves, max_depth, 1);
  delete up;
  return (b == 1 && w == 0);
}