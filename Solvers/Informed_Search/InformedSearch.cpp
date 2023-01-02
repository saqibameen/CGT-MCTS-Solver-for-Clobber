//
// Created by jfernando on 22/03/17.
//

#include "InformedSearch.h"

using namespace std;

InformedSearch::InformedSearch(Clobber *c, Database* infdata) {
  this->zh = ZobristHash();
  if (zh.RetrieveHash() == 0)
    zh.StoreHash(c);
  this->tp = new TranspositionTable();
  this->db = infdata;
}

InformedSearch::InformedSearch(Clobber *c, Database *infdata, int tablesize, int seed) {
  zh = ZobristHash(seed);
  if (zh.RetrieveHash() == 0)
    zh.StoreHash(c);
  this->tp = new TranspositionTable(tablesize);
  db = infdata;
}

InformedSearch::~InformedSearch() {
  delete tp;
}

int InformedSearch::InformedBoolSearch(Clobber *c, int depth, int player, int tp_table, int move_order) {
  this->nodes += 1;
  if (c->track == 0)
    c->SetTrack(1);
  if (depth > this->max_depth)
    this->max_depth = depth;
  vector<int> moves;
  int value = -1;
  int next_player = (player * -1) + 1;
  c->GetMoves(moves, player);
  if (move_order)
    this->OrderMoves(moves);

  /// Search the database
  if (((c->MoreThanOneSubgame() == 1) && (c->GetLargestSubgame() <= db->maxsize)) ||
      ((c->MoreThanOneSubgame() == 0) && (c->GetLargestSubgame() <= db->maxsize))) {
//    this->attempted += 1;
    vector<__int128> sgmasks;
//    clock_t t1 = clock();
    c->GetSubGames(sgmasks);
//    clock_t t2 = clock();
    int board_value = this->SearchValue(c, sgmasks, player);
//    clock_t t3 = clock();
//    this->masks_duration += t2-t1;
//    this->db_duration += t3-t2;
    if (board_value != -1) {
      this->accessed += 1;
      return board_value;
    }
  }

  /// Check if leaf node
  if (moves.empty()) {
    value = next_player; // 1 if the player is black => white wins, 0 if the player is white == black wins
    tp->KeepBestKey(zh.RetrieveHash(), -1, value, 0, player);
    return value;
  }

  /// Explore possible moves
  for (int i = 0; i < moves.size(); ++i) {
    c->Move(moves[i]);
    zh.UpdateHash(player, c->GetFrom(moves[i]), c->GetTo(moves[i]));

    /// Check Transposition Table
    if (tp_table) {
      bool intable = tp->LookUpKey(this->zh.RetrieveHash(), player);
      if (intable) {
        this->retrieved += 1;
        this->nodes += 1;
        value = tp->RetrieveValue(zh.RetrieveHash(), player);
      }
      else {
        value = this->InformedBoolSearch(c, depth - 1, next_player, tp_table, move_order);
      }
    }/// Call AB again
    else {
      value = this->InformedBoolSearch(c, depth - 1, next_player, tp_table, move_order);
    }

    tp->KeepBestKey(zh.RetrieveHash(), depth, value, moves[i], player);
    zh.UpdateHash(player, c->GetFrom(moves[i]), c->GetTo(moves[i])); // Reverses the hash value
    c->ReverseMove(moves[i]); // Reverses the board
    this->first_move = moves[i];
    if (value == player) // The player found a winning move
      break;
    }
  return value;
}

// Returns -1 if one of the games in sgmasks is not stored in the database otherwise it returns the outcome of the games
int InformedSearch::SearchValue(Clobber *g, vector<__int128> sgmasks, int player) {
  vector<string> boards;

  int checklower = 1, checkupper = 1, exact=1, j = -1;
  ASvalue gsum = ASvalue(0,0,0,0), lower = ASvalue(0,0,0,0), upper = ASvalue(0,0,0,0);

  for (int i = 0; i < sgmasks.size(); i++) {
    string temp_board = (*g).GetSubgameString(sgmasks[i]);
    bool dead = (*g).DeadSubgame(sgmasks[i]);
    if (!dead) {
      DatabaseEntry* entry = db->RetrieveEntry(temp_board);
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

int InformedSearch::EvaluateGameSum(ASvalue gsum, int player, int exact) {
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
int InformedSearch::EvaluateBoundary(ASvalue boundary, int player) {
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

// Reorders the move so that the middle one is first
void InformedSearch::OrderMoves(vector<int> &moves) {
  int move = 0;
  for (int i = 0; i < moves.size() / 2; i++) {
    move = moves.front();
    moves.erase(moves.begin());
    moves.push_back(move);
  }
}

// Eliminates games or sum of games that are no longer needed to find the outcome of the game
void InformedSearch::EliminateIrrelevantGames(Clobber *c, vector<__int128> sgmasks) {
  vector<__int128> observed_masks;
  vector<ASvalue> observed_value;

  for (int i = 0; i < sgmasks.size(); i++){
    string temp_board = c->GetSubgameString(sgmasks[i]);
    if (db->LookUpKey(temp_board)) {
      if (db->RetrieveFlag(temp_board) == 1) {
        ASvalue exact = db->RetrieveExactValue(temp_board);
        if (exact == rf.zero.value)
          c->EliminateMask(sgmasks[i]);
        else {
          if (observed_masks.empty()) {
            observed_masks.push_back(sgmasks[i]);
            observed_value.push_back(exact);
          }
          else {
            int removed = 0;
            for (int j = 0; j < observed_masks.size(); j++) {
              if (exact.Inverse() == observed_value[j]) {
                c->EliminateMask(sgmasks[i]);
                c->EliminateMask(observed_masks[j]);
                observed_masks.erase(observed_masks.begin()+j);
                observed_value.erase(observed_value.begin()+j);
                removed = 1;
                break;
              }
            }
            if (!remove)
              observed_masks.push_back(sgmasks[i]), observed_value.push_back(exact);
          }
        }
      }
    }
  }
}

/// Search Summaries
void InformedSearch::PrintSearchStats() {
  cout << "Search Statistics..." << endl;
  cout << "Nodes: " << this->nodes << endl;
  cout << "Retrieved: " << this->retrieved << endl;
  cout << "Accessed: " << this->accessed << endl;
  cout << "Table Statistics..." << endl;
  tp->PrintTableStats();
}

void InformedSearch::PrintBestMoves(Clobber *c, int player) {
  vector<int> moves;
  c->GetMoves(moves, player);
  c->PrintBoard();
  stack<int> moves_stack;
  while (!moves.empty()) {
    InformedSearch temp_search = InformedSearch(c, this->db);
    int winner = temp_search.InformedBoolSearch(c, 100, player);
    int move = temp_search.GetFirstMove();
    moves_stack.push(move);
    if (c->GetLargestSubgame() > 8) {
      cout << "Move: " << c->MoveToStr(move) << endl;
      c->Move(move);
      c->PrintBoard();
    }
    if (c->GetLargestSubgame() <= this->db->maxsize) {
      vector<string> players = {"Black", "White"};
      cout << players[player] << " wins!" << endl;
      break;
    }
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
lli InformedSearch::GetNodes() {
  return this->nodes;
}

lli InformedSearch::GetAccessed() {
  return this->accessed;
}

lli InformedSearch::GetRetrieved() {
  return this->retrieved;
}

lli InformedSearch::GetStored() {
  return tp->stored;
}

lli InformedSearch::GetCollisions() {
  return tp->collisions;
}

lli InformedSearch::GetReplaced() {
  return tp->replaced;
}

int InformedSearch::GetFirstMove() {
  return this->first_move;
}

//float InformedSearch::GetMaskTime() {
//  return ((float) this->masks_duration) / CLOCKS_PER_SEC * 1000;
//}
//
//float InformedSearch::GetDbTime() {
//  return ((float) this->db_duration) / CLOCKS_PER_SEC * 1000;
//}