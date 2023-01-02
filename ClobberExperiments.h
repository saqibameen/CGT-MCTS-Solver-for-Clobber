#ifndef CLOBBEREXPERIMENTS_H
#define CLOBBEREXPERIMENTS_H

#include "Clobber.h"
#include "Players/ClobberPlayer.h"
#include "Players/ClobberPlayerCGT.h"
#include "Players/ClobberPlayerRandom.h"
#include "Players/ClobberPlayerMCTS.h"
#include <iostream>
#include <fstream>

// declarations
void exp_decomposition_rand(string fname, int rows, int cols, int runs);
void exp_decomposition_mcts(string fname, int rows, int cols, int runs);
void cgt_vs_mcts(string fname, int rows, int cols, int runs, float time);
void cgt_vs_cgtdb(string fname, int rows, int cols, int runs, float time);
int game_loop(ClobberPlayer** players, Clobber* c, int first_player);

// experiments to run
void clobber_experiments() {
  ZobristHash::InitTable(143);
  ABSolver::InitTT();
  cgt_vs_cgtdb("Data/cgt_mcts_results/cgt_cgtdb_06.txt", 6, 6, 100, 15);
  // cgt_vs_cgtdb("Data/cgt_mcts_results/cgt_cgtdb_08.txt", 8, 8, 100, 15);
  // cgt_vs_cgtdb("Data/cgt_mcts_results/cgt_cgtdb_10.txt", 10, 10, 100, 15);
  /*
  exp_decomposition_rand("Data/cd_rand_04.txt", 4, 4, 10000);
  exp_decomposition_rand("Data/cd_rand_06.txt", 6, 6, 10000);
  exp_decomposition_rand("Data/cd_rand_08.txt", 8, 8, 10000);
  exp_decomposition_rand("Data/cd_rand_10.txt", 10, 10, 10000);
  */
  /*
  exp_decomposition_mcts("Data/cd_mcts_04.txt", 4, 4, 10);
  exp_decomposition_mcts("Data/cd_mcts_06.txt", 6, 6, 10);
  exp_decomposition_mcts("Data/cd_mcts_08.txt", 8, 8, 10);
  exp_decomposition_mcts("Data/cd_mcts_10.txt", 10, 10, 10);
  */
}

// decomposition experiments
void exp_decomposition_rand(string fname, int rows, int cols, int runs) {
  cout << "exp_decomposition " << fname << ' ' << rows << 'x' << cols << ' ' << runs << " runs...\n";
  ofstream outfile;
  outfile.open(fname);
  if (outfile.is_open()) {
    int sgs[rows * cols]; // data vector
    string to_write;
    Clobber c = Clobber(rows, cols); // clobber board
    int player; // current player
    int turns; // turn counter
    int move; // player move

    ClobberPlayer* players[2]; // players
    for (int i = 0; i < 2; i++)
      players[i] = new ClobberPlayerRandom(i);

    for (int r = 0; r < runs; r++) {
      // clear data array
      for (int i = 0; i < rows * cols; i++)
        sgs[i] = 0;
      // init game
      c.InitBoard();
      player = 0;
      turns = 0;
      // play!
      while (true) {
        move = players[player]->SelectMove(&c); // select move
        if (move == -1) // end if current player has no moves
          break;
        c.Move(move); // play move
        sgs[turns] = c.CountSubgames(); // store number of sub-games in data array
        player = !player; // change player
        turns++; // increment turns
      }
      // write array to file
      to_write = "";
      for (int i = 0; i < rows * cols; i++) {
        to_write += to_string(sgs[i]);
        if (i != rows * cols - 1)
          to_write += ',';
      }
      to_write += '\n';
      outfile << to_write;
    }
    outfile.close();
  } else
    throw "couldn't open output file";
}

void exp_decomposition_mcts(string fname, int rows, int cols, int runs) {
  cout << "exp_decomposition " << fname << ' ' << rows << 'x' << cols << ' ' << runs << " runs...\n";
  ofstream outfile;
  outfile.open(fname);
  if (outfile.is_open()) {
    int sgs[rows * cols]; // data vector
    string to_write;
    Clobber c = Clobber(rows, cols); // clobber board
    int player; // current player
    int turns; // turn counter
    int move; // player move

    ClobberPlayerMCTS* players[2]; // players
    for (int i = 0; i < 2; i++)
      players[i] = new ClobberPlayerMCTS(i, 30);

    for (int r = 0; r < runs; r++) {
      // clear data array
      cout << "new run\n";
      for (int i = 0; i < rows * cols; i++)
        sgs[i] = 0;
      // init game
      c.InitBoard();
      player = 0;
      turns = 0;
      // play!
      while (true) {
        move = players[player]->SelectMove(&c); // select move
        if (move == -1) // end if current player has no moves
          break;
        c.Move(move); // play move
        sgs[turns] = c.CountSubgames(); // store number of sub-games in data array
        player = !player; // change player
        turns++; // increment turns
      }
      // write array to file
      to_write = "";
      for (int i = 0; i < rows * cols; i++) {
        to_write += to_string(sgs[i]);
        if (i != rows * cols - 1)
          to_write += ',';
      }
      to_write += '\n';
      outfile << to_write;
    }
    outfile.close();
  } else
    throw "couldn't open output file";
}

void cgt_vs_mcts(string fname, int rows, int cols, int runs, float time) {
  Clobber::BuildMoveTables(rows, cols);
  cout << "cgt_vs_mcts " << fname << ' ' << rows << 'x' << cols << ' ' << runs << " runs...\n";
  ofstream outfile;
  outfile.open(fname, ios::app);
  Database db;
  db.LoadDatabase(0);
  if (outfile.is_open()) {
    int halfruns = runs / 2;
    string to_write;
    Clobber c = Clobber(rows, cols); // clobber board
    int winner;

    ClobberPlayer* players[2]; // players
    // NOTE: add CGT player here.
    players[0] = new ClobberPlayerCGT(0, 1);
    players[1] = new ClobberPlayerMCTS(1, 1);
    for (int fp = 0; fp < 2; fp++) {
      for (int r = 0; r < halfruns; r++) {
        cout << "new run -number:" << r << "\n";
        c.InitBoard();
        for (int i = 0; i < 2; i++)
          players[i]->ClearTable();
        winner = game_loop(players, &c, fp);
        outfile << to_string(winner == 0) << ',';
      }
    }
    outfile.close();
  } else
    throw "couldn't open output file";
  Clobber::DeleteMoveTables(rows, cols);
}

/*
  Compare CGT vs CGTDB here.
*/
void cgt_vs_cgtdb(string fname, int rows, int cols, int runs, float time) {
  Clobber::BuildMoveTables(rows, cols);
  cout << "cgt_vs_cgtdb " << fname << ' ' << rows << 'x' << cols << ' ' << runs << " runs...\n";
  Database db;
  db.LoadDatabase(0);
  int halfruns = runs / 2;
  string to_write;
  Clobber c = Clobber(rows, cols); // clobber board
  int winner;

  ClobberPlayer* players[2]; // players
  // NOTE: add CGT player here.
  players[0] = new ClobberPlayerCGTDB(0, time, &db); // Black -> CGTDB
  players[1] = new ClobberPlayerCGT(1, time); // White -> CGT
  for (int fp = 0; fp < 2; fp++) {
    for (int r = 0; r < 1; r++) {
      // cout << "new run\n";
      c.InitBoard();
      for (int i = 0; i < 2; i++)
        players[i]->ClearTable();
      winner = game_loop(players, &c, fp);
      string player_names = "BW";
      cout << "winner:" << player_names[winner] << " wins\n";
    }
  }
  Clobber::DeleteMoveTables(rows, cols);
}

int game_loop(ClobberPlayer** players, Clobber* c, int first_player) {
  string player_names = "BW";
  int player = first_player;
  int turns = 0;
  int move;
  while (true) {
    turns++;
    move = players[player]->SelectMove(c);
    if (move == -1)
      break;
    c->Move(move);
    cout << turns << ": " << player_names[player] << ' ' << c->MoveToStr(move) << '\n';
    player = !player;
  }
  return !player;
}

#endif