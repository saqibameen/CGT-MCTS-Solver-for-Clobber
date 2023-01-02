#include "ClobberPlayerCGT.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

int ClobberPlayerCGT::RandInt(int lb, int ub) {
  uniform_int_distribution<int> dist(lb, ub);
  return dist(this->rng);
}

void ClobberPlayerCGT::InitTable() {
  this->htable = new unsigned int*[CGT_HASH_CAPACITY]();
  for (int i = 0; i < CGT_HASH_CAPACITY; i++)
    this->htable[i] = new unsigned int[2]();
}

void ClobberPlayerCGT::ClearTable() {
  for (int i = 0; i < CGT_HASH_CAPACITY; i++)
    for (int j = 0; j < 2; j++)
      this->htable[i][j] = 0;
}

ClobberPlayerCGT::ClobberPlayerCGT(int color, float t_turn) {
  random_device rd;
  this->rng = mt19937(rd());
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
}

ClobberPlayerCGT::ClobberPlayerCGT(int seed, int color, float t_turn) {
  this->rng = mt19937(seed);
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
}

int ClobberPlayerCGT::SelectMove(Clobber* c) {
  time_t t1 = time(0);
  c->GetMoves(this->moves, this->color);

  if (this->moves.empty()) // player lost
    return -1;
  
  if (this->moves.size() == 1) { // no need to search with 1 move left
    int move = this->moves[0];
    this->moves.clear();
    return move;
  }

  // filter zeros from board
  vector<__int128> sgmasks; vector<int> sg_sizes;
  vector<int> star; vector<int> up; vector<int> down;
  vector<int> upstar; vector<int> downstar;
  vector<int> up2star; vector<int> down2star;
  Clobber* c_eq = new Clobber(*c);
  Clobber* csg;
  int inf, removed = 0;
  int max_i, max_size;
  c->GetSubgames(sgmasks); c_eq->FilterBoard(sgmasks);
  if (sgmasks.size() > 1) {
    for (int i = 0; i < sgmasks.size(); i++) {
      if (removed + 1 == sgmasks.size())
        break;
      sg_sizes.push_back(this->MeasureSubgame(sgmasks[i]));
      if (sg_sizes[i] <= CGT_MAX_SG_SIZE) {
        csg = c->GetSubgame(sgmasks[i]);
        inf = ABSolver::SimpleInf(csg, CGT_MAX_SG_SIZE);
        switch (inf) {
          case 0: // 0
            if (removed + 1 < sgmasks.size()){c_eq->RemoveSubgame(sgmasks[i]); removed++;} break; 
          case 1: // *
            star.push_back(i); break;
          case 2: // ↑
            up.push_back(i); break;
          case 3: // ↓
            down.push_back(i); break;
          case 4: // ↑*
            upstar.push_back(i); break;
          case 5: // ↓*
            downstar.push_back(i); break;
          case 6: // ↑↑*
            up2star.push_back(i); break;
          case 7: // ↓↓*
            down2star.push_back(i); break;
        }
        delete csg;
      }
    }
    if (removed > 0)
      // cout << "✖ " << removed << ": 0s\n";
    while (removed + 4 < sgmasks.size() && up2star.size() >= 1 && star.size() >= 1 && down.size() >= 2) {
      max_i = 0; max_size = sg_sizes[up2star[0]];
      for (int i = 1; i < up2star.size(); i++)
        if (sg_sizes[up2star[i]] > max_size) { max_i = i; max_size = sg_sizes[up2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[up2star[max_i]]); up2star.erase(up2star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down[0]];
      for (int i = 1; i < down.size(); i++)
        if (sg_sizes[down[i]] > max_size) { max_i = i; max_size = sg_sizes[down[i]]; }
      c_eq->RemoveSubgame(sgmasks[down[max_i]]); down.erase(down.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down[0]];
      for (int i = 1; i < down.size(); i++)
        if (sg_sizes[down[i]] > max_size) { max_i = i; max_size = sg_sizes[down[i]]; }
      c_eq->RemoveSubgame(sgmasks[down[max_i]]); down.erase(down.begin() + max_i);
      removed += 4;
      // cout << "✖ 4: ↑↑*, *, ↓, ↓\n";
    }
    while (removed + 4 < sgmasks.size() && down2star.size() >= 1 && star.size() >= 1 && up.size() >= 2) {
      max_i = 0; max_size = sg_sizes[down2star[0]];
      for (int i = 1; i < down2star.size(); i++)
        if (sg_sizes[down2star[i]] > max_size) { max_i = i; max_size = sg_sizes[down2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[down2star[max_i]]); down2star.erase(down2star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[up[0]];
      for (int i = 1; i < up.size(); i++)
        if (sg_sizes[up[i]] > max_size) { max_i = i; max_size = sg_sizes[up[i]]; }
      c_eq->RemoveSubgame(sgmasks[up[max_i]]); up.erase(up.begin() + max_i);
      max_i = 0; max_size = sg_sizes[up[0]];
      for (int i = 1; i < up.size(); i++)
        if (sg_sizes[up[i]] > max_size) { max_i = i; max_size = sg_sizes[up[i]]; }
      c_eq->RemoveSubgame(sgmasks[up[max_i]]); up.erase(up.begin() + max_i);
      removed += 4;
      // cout << "✖ 4: ↓↓*, *, ↑, ↑\n";
    }
    while (removed + 3 < sgmasks.size() && up2star.size() >= 1 && downstar.size() >= 1 && down.size() >= 1) {
      max_i = 0; max_size = sg_sizes[up2star[0]];
      for (int i = 1; i < up2star.size(); i++)
        if (sg_sizes[up2star[i]] > max_size) { max_i = i; max_size = sg_sizes[up2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[up2star[max_i]]); up2star.erase(up2star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[downstar[0]];
      for (int i = 1; i < downstar.size(); i++)
        if (sg_sizes[downstar[i]] > max_size) { max_i = i; max_size = sg_sizes[downstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[downstar[max_i]]); downstar.erase(downstar.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down[0]];
      for (int i = 1; i < down.size(); i++)
        if (sg_sizes[down[i]] > max_size) { max_i = i; max_size = sg_sizes[down[i]]; }
      c_eq->RemoveSubgame(sgmasks[down[max_i]]); down.erase(down.begin() + max_i);
      removed += 3;
      // cout << "✖ 3: ↑↑*, ↓*, ↓\n";
    }
    while (removed + 3 < sgmasks.size() && down2star.size() >= 1 && upstar.size() >= 1 && up.size() >= 1) {
      max_i = 0; max_size = sg_sizes[down2star[0]];
      for (int i = 1; i < down2star.size(); i++)
        if (sg_sizes[down2star[i]] > max_size) { max_i = i; max_size = sg_sizes[down2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[down2star[max_i]]); down2star.erase(down2star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[upstar[0]];
      for (int i = 1; i < upstar.size(); i++)
        if (sg_sizes[upstar[i]] > max_size) { max_i = i; max_size = sg_sizes[upstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[upstar[max_i]]); upstar.erase(upstar.begin() + max_i);
      max_i = 0; max_size = sg_sizes[up[0]];
      for (int i = 1; i < up.size(); i++)
        if (sg_sizes[up[i]] > max_size) { max_i = i; max_size = sg_sizes[up[i]]; }
      c_eq->RemoveSubgame(sgmasks[up[max_i]]); up.erase(up.begin() + max_i);
      removed += 3;
      // cout << "✖ 3: ↓↓*, ↑*, ↑\n";
    }
    while (removed + 2 < sgmasks.size() && up2star.size() >= 1 && down2star.size() >= 1) {
      max_i = 0; max_size = sg_sizes[up2star[0]];
      for (int i = 1; i < up2star.size(); i++)
        if (sg_sizes[up2star[i]] > max_size) { max_i = i; max_size = sg_sizes[up2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[up2star[max_i]]); up2star.erase(up2star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down2star[0]];
      for (int i = 1; i < down2star.size(); i++)
        if (sg_sizes[down2star[i]] > max_size) { max_i = i; max_size = sg_sizes[down2star[i]]; }
      c_eq->RemoveSubgame(sgmasks[down2star[max_i]]); down2star.erase(down2star.begin() + max_i);
      removed += 2;
      // cout << "✖ 2: ↑↑*, ↓↓*\n";
    }
    while (removed + 3 < sgmasks.size() && upstar.size() >= 1 && star.size() >= 1 && down.size() >= 1) {
      max_i = 0; max_size = sg_sizes[upstar[0]];
      for (int i = 1; i < upstar.size(); i++)
        if (sg_sizes[upstar[i]] > max_size) { max_i = i; max_size = sg_sizes[upstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[upstar[max_i]]); upstar.erase(upstar.begin() + max_i);
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down[0]];
      for (int i = 1; i < down.size(); i++)
        if (sg_sizes[down[i]] > max_size) { max_i = i; max_size = sg_sizes[down[i]]; }
      c_eq->RemoveSubgame(sgmasks[down[max_i]]); down.erase(down.begin() + max_i);
      removed += 3;
      // cout << "✖ 3: ↑*, *, ↓\n";
    }
    while (removed + 3 < sgmasks.size() && downstar.size() >= 1 && star.size() >= 1 && up.size() >= 1) {
      max_i = 0; max_size = sg_sizes[downstar[0]];
      for (int i = 1; i < downstar.size(); i++)
        if (sg_sizes[downstar[i]] > max_size) { max_i = i; max_size = sg_sizes[downstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[downstar[max_i]]); downstar.erase(downstar.begin() + max_i);
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[up[0]];
      for (int i = 1; i < up.size(); i++)
        if (sg_sizes[up[i]] > max_size) { max_i = i; max_size = sg_sizes[up[i]]; }
      c_eq->RemoveSubgame(sgmasks[up[max_i]]); up.erase(up.begin() + max_i);
      removed += 3;
      // cout << "✖ 3: ↓*, *, ↑\n";
    }
    while (removed + 2 < sgmasks.size() && upstar.size() >= 1 && downstar.size() >= 1) {
      max_i = 0; max_size = sg_sizes[upstar[0]];
      for (int i = 1; i < upstar.size(); i++)
        if (sg_sizes[upstar[i]] > max_size) { max_i = i; max_size = sg_sizes[upstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[upstar[max_i]]); upstar.erase(upstar.begin() + max_i);
      max_i = 0; max_size = sg_sizes[downstar[0]];
      for (int i = 1; i < downstar.size(); i++)
        if (sg_sizes[downstar[i]] > max_size) { max_i = i; max_size = sg_sizes[downstar[i]]; }
      c_eq->RemoveSubgame(sgmasks[downstar[max_i]]); downstar.erase(downstar.begin() + max_i);
      removed += 2;
      // cout << "✖ 2: ↑*, ↓*\n";
    }
    while (removed + 2 < sgmasks.size() && up.size() >= 1 && down.size() >= 1) {
      max_i = 0; max_size = sg_sizes[up[0]];
      for (int i = 1; i < up.size(); i++)
        if (sg_sizes[up[i]] > max_size) { max_i = i; max_size = sg_sizes[up[i]]; }
      c_eq->RemoveSubgame(sgmasks[up[max_i]]); up.erase(up.begin() + max_i);
      max_i = 0; max_size = sg_sizes[down[0]];
      for (int i = 1; i < down.size(); i++)
        if (sg_sizes[down[i]] > max_size) { max_i = i; max_size = sg_sizes[down[i]]; }
      c_eq->RemoveSubgame(sgmasks[down[max_i]]); down.erase(down.begin() + max_i);
      removed += 2;
      // cout << "✖ 2: ↑, ↓\n";
    }
    while (removed + 2 < sgmasks.size() && star.size() >= 2) {
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      max_i = 0; max_size = sg_sizes[star[0]];
      for (int i = 1; i < star.size(); i++)
        if (sg_sizes[star[i]] > max_size) { max_i = i; max_size = sg_sizes[star[i]]; }
      c_eq->RemoveSubgame(sgmasks[star[max_i]]); star.erase(star.begin() + max_i);
      removed += 2;
      // cout << "✖ 2: *, *\n";
    }
    if (removed > 0) {
      // cout << "cgt equiv: \n";
      // c_eq->PrintBoard();
    }
  }
  
  // no need to search if 1 move left (after CGT filtering)
  this->moves.clear(); c_eq->GetMoves(this->moves, this->color);
  if (this->moves.size() == 1) {
    int move = this->moves[0];
    this->moves.clear();
    delete c_eq;
    return move;
  }

  CGTNode* node;
  this->tree = new CGTNode(NULL, new Clobber(*c_eq), -1, this->color, this->htable, ZobristHash::ComputeHash(c_eq)); // make tree
  
  // do MCTS for the alotted time
  time_t duration;
  for (int i = 0; i < CGT_MAX_SELECTIONS; i++) {
    duration = (float)(time(0) - t1);
    if (duration < this->t_turn) {
      node = this->tree->Selection();
      for (int j = 0; j < CGT_N_SIMS; j++)
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
  
    // cout << "v: " << best_val << ", n: " << this->htable[this->tree->key % CGT_HASH_CAPACITY][1] << '\n';
  this->moves.clear();
  delete this->tree;
  delete c_eq;
  return move;
}

void ClobberPlayerCGT::Simulation(CGTNode* node, int player) {
  Clobber* c = new Clobber(*(node->s));
  int turn = player;
  vector<int> moves = node->moves;
  int move;
  while (true) {
    if (moves.empty())
      break;
    move = this->RandInt(0, moves.size() - 1);
    c->Move(moves[move]);
    c->UpdateMoves(moves, moves[move]);
    turn = !turn;
  }
  delete c;
  node->Backprop(player == turn, !player);
  return;
}

int ClobberPlayerCGT::MeasureSubgame(__int128 sgmask) {
  int count = 0;
  while (sgmask != 0) {
    if (sgmask & 1)
      count++;
    sgmask >>= 1;
  }
  return count;
}