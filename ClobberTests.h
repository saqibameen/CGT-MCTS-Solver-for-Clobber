#ifndef CLOBBERTESTS_H
#define CLOBBERTESTS_H

#include "Clobber.h"
#include "ZobristHash.h"
#include "ABSolver.h"
#include <iostream>
#include <time.h>
#include <math.h>

void clobber_tests() {
  vector<int> moves;
  vector<__int128> sgmasks;

  // make a 4x4 board
  Clobber::BuildMoveTables(4, 4);
  cout << "Creating a 4x4 Clobber position...\n\n";
  Clobber c = Clobber(4, 4);
  c.PrintBoard();
  cout << "---\n";

  // test out some moves
  cout << "Performing moves...\n\n";
  cout << "B move a1b1\n";
  c.Move("a1b1");
  c.PrintBoard();
  cout << "W move c2c1\n";
  c.Move("c2c1");

  c.PrintBoard();
  cout << "---\n";

  // get the string of the current board
  cout << "Getting board string...\n\n";
  cout << "board str: " << c.ToString() << '\n';
  cout << "---\n";

  // get black's moves for the current state
  cout << "Getting black player's moves...\n\n";
  cout << "black's moves: ";
  c.GetMoves(moves, 0);
  for (int i = 0; i < moves.size(); i++)
    cout << c.MoveToStr(moves[i]) << ' ';
  cout << '\n';
  cout << "---\n";

  // update moves
  cout << "Incrementally updating moves...\n\n";
  cout << "B move " << c.MoveToStr(moves[3]) << '\n';
  c.Move(moves[3]);
  c.PrintBoard();
  cout << "white's moves: ";
  c.UpdateMoves(moves, moves[3]);
  for (int i = 0; i < moves.size(); i++)
    cout << c.MoveToStr(moves[i]) << ' ';
  cout << '\n';
  cout << "---\n";

  // test hashing
  cout << "Testing Zobrist hashing...\n\n";
  ZobristHash::InitTable(143);
  unsigned long long int hash = ZobristHash::ComputeHash(&c);
  cout << "current hash: " << hash << '\n';
  cout << "move " << c.MoveToStr(moves[2]) << '\n';
  c.Move(moves[2]);
  hash = ZobristHash::ComputeHash(hash, 1, c.GetFrom(moves[2]), c.GetTo(moves[2]));
  cout << "updated hash: " << hash << '\n';
  cout << "recomputed hash: " << ZobristHash::ComputeHash(&c) << '\n';
  hash = ZobristHash::ComputeHash(hash, 1, c.GetFrom(moves[2]), c.GetTo(moves[2]));
  cout << "undo hash: " << hash << '\n';
  cout << "---\n";

  // load a board from a string
  Clobber::DeleteMoveTables(4, 4);
  Clobber::BuildMoveTables(6, 5);
  cout << "Loading a Clobber position from a string...\n\n";
  cout << "loading r6c5x.o.oxx.xooo.x.x.o.oox..x.o.x.\n";
  c = Clobber("r6c5x.o.oxx.xooo.x.x.o.oox..x.o.x.");
  c.PrintBoard();
  cout << "---\n";

  // subgames
  cout << "Getting subgames from previous board...\n\n";
  Clobber* sg;
  c.GetSubgames(sgmasks);
  for (int i = 0; i < sgmasks.size(); i++) {
    sg = c.GetSubgame(sgmasks[i]);
    sg->PrintBoard();
    delete sg;
  }
  cout << "filtering board\n";
  c.FilterBoard(sgmasks);
  c.PrintBoard();
  cout << "removing 2nd subgame\n";
  c.RemoveSubgame(sgmasks[1]);
  c.PrintBoard();
  cout << "---\n";
  
  // ab solver
  Clobber::DeleteMoveTables(6, 5);
  ABSolver::InitTT();
  cout << "Testing AB solver...\n\n";
  string outcomes = "PNLR";
  for (int i = 3; i < 7; i++) {
    Clobber::BuildMoveTables(3, i);
    c = Clobber(3, i);
    c.PrintBoard();
    cout << "Outcome class: " << outcomes[ABSolver::OutcomeClass(&c, 20)] << '\n';
    Clobber::DeleteMoveTables(3, i);
  }
  cout << "---\n";

  // testing infinitesimal classifiers
  cout << "Testing infinitesimal classifiers...\n";
  Clobber::BuildMoveTables(1, 5);
  vector<string> infs;
  infs.push_back("0"); infs.push_back("*"); infs.push_back("↑"); infs.push_back("↓");
  infs.push_back("↑*"); infs.push_back("↓*"); infs.push_back("↑↑*"); infs.push_back("↓↓*");
  infs.push_back("±(↑,*)");
  c = Clobber("r1c5.xxoo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5...xo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5..xxo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5..xoo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5.oxoo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5.xoxx");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5.oxxx");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5.xooo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  c = Clobber("r1c5.xoxo");
  c.PrintBoard();
  cout << "Inf: " << infs[ABSolver::SimpleInf(&c, 16)] << '\n';
  Clobber::DeleteMoveTables(1, 5);
  cout << "---\n";
}

#endif