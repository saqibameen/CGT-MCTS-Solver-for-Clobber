#include "ClobberPlayerCGTDB.h"
#include "../Thermographs/ASvalue.h"
#include "../Thermographs/Database.h"
#include "../Thermographs/DatabaseEntry.h"
#include "../Thermographs/InfGamesDefinitions.h"
#include "../Thermographs/InfinitesimalGame.h"

#include <stdlib.h>
#include <iostream>

using namespace std;

int ClobberPlayerCGTDB::RandInt(int lb, int ub) {
  uniform_int_distribution<int> dist(lb, ub);
  return dist(this->rng);
}

void ClobberPlayerCGTDB::InitTable() {
  this->htable = new unsigned int*[CGT_HASH_CAPACITY]();
  for (int i = 0; i < CGT_HASH_CAPACITY; i++)
    this->htable[i] = new unsigned int[2]();
}

void ClobberPlayerCGTDB::ClearTable() {
  for (int i = 0; i < CGT_HASH_CAPACITY; i++)
    for (int j = 0; j < 2; j++)
      this->htable[i][j] = 0;
}

ClobberPlayerCGTDB::ClobberPlayerCGTDB(int color, float t_turn, Database *infdata) {
  random_device rd;
  this->rng = mt19937(rd());
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
  this->db = infdata;
}

ClobberPlayerCGTDB::ClobberPlayerCGTDB(int seed, int color, float t_turn, Database *infdata) {
  this->rng = mt19937(seed);
  this->color = color;
  this->moves.clear();
  this->t_turn = t_turn;
  this->InitTable();
  db = infdata;
}

// Returns -1 if one of the games in sgmasks is not stored in the database otherwise it returns the outcome of the games
int ClobberPlayerCGTDB::SearchValue(Clobber *g, vector<__int128> sgmasks, int player) {
  vector<string> boards;

  int checklower = 1, checkupper = 1, exact=1, j = -1;
  ASvalue gsum = ASvalue(0,0,0,0), lower = ASvalue(0,0,0,0), upper = ASvalue(0,0,0,0);

  // cout << "Original Board" << endl;
  // g->PrintBoard();
  // cout << endl;

  for (int i = 0; i < sgmasks.size(); i++) {
    string temp_board = (*g).GetSubgameString(sgmasks[i]);

    Clobber temp_clobber = Clobber(temp_board, 0);

    bool dead = (*g).DeadSubgame(sgmasks[i]);
    if (!dead) {
      DatabaseEntry* entry = this->db->RetrieveEntry(temp_board);

      // cout << endl;
      // temp_clobber.PrintBoard();
      // cout << endl;
      // cout << "Board: " << entry->board << endl;
      // cout << "Temp Board: " << temp_board << endl;
      if (entry == NULL)
        return -1;
      else if (entry != NULL) {
        if (entry->flag == 1) {
          gsum = gsum + entry->exact_value;
        } else if (entry->flag == 0) {
          exact = 0;
          if (player == 0)
            gsum = gsum + entry->lower_bound;
          else if (player == 1)
            gsum = gsum + entry->upper_bound;
        }
        if (entry->upper_bound == rf.unkn.value)
          checkupper = 0;
        if (entry->lower_bound == rf.unkn.value)
          checklower = 0;
        lower = lower + entry->lower_bound;
        upper = upper + entry->upper_bound;
      }
    }
  }

  if (checklower == 0 && checkupper == 0 && exact == 0)
    return -1;

  if ((player == 0 && checklower) || (player == 1 && checkupper) || exact) {
    int result = EvaluateGameSum(gsum, player, exact);
    if (exact)
      return result;
    else if (player == result)
      return result;
  }

  if (player == 0 && checkupper)
    return EvaluateBoundary(upper, player);
  else if (player == 1 && checklower)
    return EvaluateBoundary(lower, player);
  return -1;
}

/*
  Algorithm # 04 from the thesis.
*/
int ClobberPlayerCGTDB::dbResultEvaluation(int board_value, int playerToPlay, int nextPlayerToPlay){
  int to_win;
  if (board_value > 0){
    to_win = 0;
  }else if (board_value < 0){
    to_win = 1;
  }else if (board_value == 0){ // if the value is zero.
    if(nextPlayerToPlay == 0){ // black is next to move
      to_win = 1;
    }else if (nextPlayerToPlay == 1){ // white is next to move
      to_win = 0;
    } 
  }else if (board_value == -1){ // is the value is fuzzy
    if(nextPlayerToPlay == 0){ // black is next to move
      to_win = 0;
    }else if (nextPlayerToPlay == 1){ // white is next to move
      to_win = 1;
    }
  }else{ // the value is unknown
    return -1;
  }
  return to_win;
}

int ClobberPlayerCGTDB::EvaluateGameSum(ASvalue gsum, int player, int exact) {
  if (gsum == (this->rf.zero.value) && exact)
    return (player - 1)*-1;
  else if (gsum.u >= 2)
    return 0;
  else if (gsum.u <= -2)
    return 1;
  else if ((gsum.u >= 1 || (gsum.u2 >= 1 && gsum.u >= 0) ||
          (gsum.u3 >= 1 && gsum.u2 >= 0 && gsum.u >= 0) ) && (gsum.star == 0))
    return 0;
  else if ((gsum.u <= -1 || (gsum.u2 <= -1 && gsum.u <=0) ||
          (gsum.u3 <= -1 && gsum.u2 <=0 && gsum.u <=0)) && (gsum.star == 0))
    return 1;
  else if (gsum.u <= 1 && gsum.u >= -1 && gsum.star == 1)
    return player;
  return -1;
}

// Evaluates the boundary of the game
int ClobberPlayerCGTDB::EvaluateBoundary(ASvalue boundary, int player) {
  int opposite_player = (player - 1) * -1;
  if (boundary == (this->rf.zero.value))
    return opposite_player;
  else if ((boundary.u >= 1 || (boundary.u2 >= 1 && boundary.u >= 0) ||
          (boundary.u3 >= 1 && boundary.u2 >= 0 && boundary.u >= 0))
           && (boundary.star == 0) && (player == 1)){
    return opposite_player;
  }
  else if ((boundary.u <= -1 || (boundary.u2 <= -1 && boundary.u <= 0) ||
          (boundary.u3 <= -1 && boundary.u2 <= 0 && boundary.u3 <= 0))
           && (boundary.star ==0) && (player==0))
    return opposite_player;
  return -1;
}

/*
  Checks if the node exists in the DB.
*/
int ClobberPlayerCGTDB::getBoardValueFromDB(CGTDBNode* node){
  Clobber* c = new Clobber(*(node->s));

  //Finding largest subgame
  // c->FindLargestSubgame();
  if (c->track == 0)
    c->SetTrack(1);
  if (((c->MoreThanOneSubgame() == 1) && (c->GetLargestSubgame() <= db->maxsize)) ||
      ((c->MoreThanOneSubgame() == 0) && (c->GetLargestSubgame() <= db->maxsize))) {

      // exit(0);
    vector<__int128> sgmasks;
    c->GetSubgames(sgmasks);
    int board_value = this->SearchValue(c, sgmasks, node->player);
    // cout << "Board Value: " << board_value << endl;
    // write the MCTS solver here.
    if (board_value != -1) {


      // cout << "GAME: " << endl;
      // c->PrintBoard();
      // cout << "Game string: " << c->ToString() << endl;
      // cout << "Board Value: " << board_value << endl;

      // exit(0);
      // the game is fuzzy
      return board_value;
    }
  }
  return -1;
}



int ClobberPlayerCGTDB::SelectMove(Clobber* c) {
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
        // add database check here
        // build a dummy database and add it here.
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

  CGTDBNode* node;
  this->tree = new CGTDBNode(NULL, new Clobber(*c_eq), -1, this->color, this->htable, ZobristHash::ComputeHash(c_eq)); // make tree
  
  // do MCTS for the alotted time
  time_t duration;
  for (int i = 0; i < CGT_MAX_SELECTIONS; i++) {
    duration = (float)(time(0) - t1);
    if (duration < this->t_turn) {
      node = this->tree->Selection();

      // cout << "Node player: " << node->player << endl;
      
      // Set CGT_USE_DB to true to use the database
      if(CGT_USE_DB){
        // cout << "Using DB" << endl;
        // boardValue: 0 = Black wins, 1 = White wins, -1 = Fuzzy
        int boardValue = getBoardValueFromDB(node);
        float inf = std::numeric_limits<float>::infinity();
        if(boardValue != -1){
          // proven win for the parent
          if (boardValue == !node->player){ 
          // backprop loss
            // node->BackpropProvenWin(inf);
            node->BackpropProvenLoss(inf);
          } else { // proven lose for the parent
            // backprop win
            // cout << "proven lose for the parent" << endl;
            node->BackpropProvenWin(-inf);
            // node->BackpropProvenLoss(-inf);
          }
          continue;
        }
      }
      
      for (int j = 0; j < CGT_N_SIMS; j++)
        this->Simulation(node, !node->player);
    } else {
      break;
    }
  }

  // pick the best move
  // add the move selection part here.
  int move = 0;
  float best_val = -1, val;
  for (int i = 0; i < this->tree->sp.size(); i++) {
    val = this->tree->sp[i]->SecureChildValue();
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



void ClobberPlayerCGTDB::Simulation(CGTDBNode* node, int player) {
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
  // Turn is the winner of the game, not the currrent player
  delete c;
  node->Backprop(player == turn);
  return;
}

int ClobberPlayerCGTDB::MeasureSubgame(__int128 sgmask) {
  int count = 0;
  while (sgmask != 0) {
    if (sgmask & 1)
      count++;
    sgmask >>= 1;
  }
  return count;
}