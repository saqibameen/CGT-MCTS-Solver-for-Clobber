#include "Clobber.h"
#include "ZobristHash.h"
#include "ABSolver.h"
#include "Players/ClobberPlayer.h"
#include "Players/ClobberPlayerHuman.h"
#include "Players/ClobberPlayerRandom.h"
#include "Players/ClobberPlayerMCTS.h"
#include "Players/ClobberPlayerCGT.h"
#include "Players/ClobberPlayerCGTDB.h"
#include "Players/ClobberPlayerDiff.h"

#include "ClobberTests.h"
#include "ClobberExperiments.h"

#include <iostream>
#include <fstream>

using namespace std;

// function declarations
int game_loop(ClobberPlayer** players, Clobber* c, bool verbose);

// program entry point
int main(int argc, char** argv) {
  // default values
  int board_rows = 8;
  int board_cols = 8;
  bool verbose = true;
  bool run_tests = false;
  bool run_exps = false;
  ClobberPlayer* players[2] = {NULL, NULL};
  Database db;
  db.LoadDatabase(0);

  int w_wins = 0;
  int b_wins = 0;
  for (int i = 0; i < 1; i++)
  {
      // handle args
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (string(argv[i]) == "-r") { // rows
        board_rows = stoi(argv[++i]);
      } else if (string(argv[i]) == "-c") { // cols
        board_cols = stoi(argv[++i]);
      } else if (string(argv[i]) == "-b") { // black player
        string pb_str = string(argv[++i]);
        if (pb_str == "human") {
          players[0] = new ClobberPlayerHuman(0);
        } else if (pb_str == "random") {
          players[0] = new ClobberPlayerRandom(0);
        } else if (pb_str == "mcts") {
          players[0] = new ClobberPlayerMCTS(0, stof(argv[++i]));
        } else if (pb_str == "cgt") {
          players[0] = new ClobberPlayerCGT(0, stof(argv[++i]));
        }else if (pb_str == "cgtdb") {
          players[0] = new ClobberPlayerCGTDB(0, stof(argv[++i]), &db);
        } else if (pb_str == "diff") {
          players[0] = new ClobberPlayerDiff(0);
        }
      } else if (string(argv[i]) == "-w") { // white player
        string pw_str = string(argv[++i]);
        if (pw_str == "human") {
          players[1] = new ClobberPlayerHuman(1);
        } else if (pw_str == "random") {
          players[1] = new ClobberPlayerRandom(1);
        } else if (pw_str == "mcts") {
          players[1] = new ClobberPlayerMCTS(1, stof(argv[++i]));
        } else if (pw_str == "cgt") {
          players[1] = new ClobberPlayerCGT(1, stof(argv[++i]));
        } else if (pw_str == "cgtdb") {
          players[1] = new ClobberPlayerCGTDB(1, stof(argv[++i]), &db);
        } else if (pw_str == "diff") {
          players[1] = new ClobberPlayerDiff(1);
        }
      } else if (string(argv[i]) == "-shh") { // verbose
        verbose = false;
      } else if (string(argv[i]) == "-tests") { // tests
        run_tests = true;
      } else if (string(argv[i]) == "-exps") { // experiments
        run_exps = true;
      }
    }
  }

  // if -tests, run tests and exit
  if (run_tests) {
    clobber_tests();
    return 0;
  }

  // if -exps, run experiments and exit
  if (run_exps) {
    clobber_experiments();
    return 0;
  }

  // init zobrist hashing
  ZobristHash::InitTable(143);

  // build clobber move tables
  Clobber::BuildMoveTables(board_rows, board_cols);

  // init AB transposition table
  ABSolver::InitTT();

  // fill in unspecified players with random players
  for (int i = 0; i < 2; i++)
    if (players[i] == NULL)
      players[i] = new ClobberPlayerRandom(i);

  // run game loop
  Clobber c = Clobber(board_rows, board_cols);
  //c = Clobber("r8c8oo...x..o.x....x.o.....o.o..o.x...o..xx.x..x...xo.....o.x...xo..");
  int winner = game_loop(players, &c, verbose);

  // output winner
  string player_names = "BW";
  cout << player_names[winner] << " wins\n";

  if(player_names[winner] == 'B')
    b_wins++;
  else
    w_wins++;

  // cleanup
  Clobber::DeleteMoveTables(board_rows, board_cols);
  ABSolver::DeleteTT();
  delete players[0];
  delete players[1];

  // cout << "Black wins: " << b_wins << " White wins: " << w_wins << endl;
  // cout << "B win ratio: " << (float)b_wins / 100 << endl;
  // cout << "W win ratio: " << (float)w_wins / 100 << endl;
  }

  // print win ratio and count.
  cout << "Black wins: " << b_wins << " White wins: " << w_wins << endl;
  cout << "B win ratio: " << (float)b_wins / 100 << endl;
  cout << "W win ratio: " << (float)w_wins / 100 << endl;
  

  return 0;
}

// play a game of clobber
int game_loop(ClobberPlayer** players, Clobber* c, bool verbose) {
  string player_names = "BW";
  int player = 0;
  int turns = 0;
  int move;
  if (verbose)
    c->PrintBoard();
  while (true) {
    turns++;
    move = players[player]->SelectMove(c);
    if (move == -1)
      break;
    c->Move(move);
    cout << turns << ": " << player_names[player] << ' ' << c->MoveToStr(move) << '\n';
    if (verbose)
      c->PrintBoard();
    player = !player;
  }
  return !player;
}
