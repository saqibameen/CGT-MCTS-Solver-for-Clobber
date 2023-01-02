# CMPUT 655 Project - A CGT-Aware MCTS Clobber Player

## Files:

* README.md : This file
* Clobber.cpp/.h : Clobber framework
* ABSolver.cpp/.h : Boolean Alpha-Beta solver for computing outcome classes and classifying infinitesimals
* ZobristHash.cpp/.h : Handles computing Zobrist hash values and incrementally updating them
* ClobberExperiments.h : Code for running experiments through the -exps flag
* ClobberTests.h : Code for running tests through the -tests flag
* Main.cpp : The interface to the framework that manages the input arguments
* makefile : The makefile for building the framework
* /Players
  * ClobberPlayerRandom.cpp/.h : Random player
  * ClobberPlayerHuman.cpp/.h : Human player
  * ClobberPlayerDiff.cpp/.h : No-search player that uses a piece-differential evaluation function
  * ClobberPlayerMCTS.cpp/.h : MCTS (UCT) player
  * ClobberPlayerCGT.cpp/.h : CGT-aware MCTS player
* /Data
  * inf_distribution.txt : Frequencies of infinitesimals on small positions counted from J. Fernando's database
  * mcts_playouts_per_sec_8x8.txt : Count of MCTS playouts per second for each move on an 8x8 game
  * /cgt_mcts_results : Results from putting the CGT-aware player against the MCTS player
  * /decom_results : Board decomposition results under random play and under MCTS play
  * /interesting_games : Various games I thought were worth saving for later review

## Usage

### Flags

* -r R = sets board rows to R (default: 8)
* -c C = sets board cols to C (default: 8)
* -b player_type = sets black to player_type (default: random)
* -w player_type = sets white to player_type (default: random)
* -shh = suppress output
* -tests = run tests (clobber_tests() of ClobberTests.h)
* -exps = run experiments (clobber_experiments() of ClobberExperiments.h)

### Player Types

* human
* random
* mcts N (MCTS player with N seconds per turn)
* cgt N (CGT-MCTS player with N seconds per turn)

### Example

To play a game on a 10x10 board, the CGT-MCTS player on black, the MCTS player on white, and allowing 15 seconds per move for each player:

```bash
make
./main -b cgt 15 -w mcts 15 -r 10 -c 10
```
