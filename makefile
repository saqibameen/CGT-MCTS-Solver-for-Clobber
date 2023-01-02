CC = g++
CFLAGS = -std=c++11

main :
	$(CC) $(CFLAGS) Main.cpp Clobber.cpp ZobristHash.cpp ABSolver.cpp Players/ClobberPlayerRandom.cpp Players/ClobberPlayerHuman.cpp Players/ClobberPlayerMCTS.cpp Players/ClobberPlayerCGT.cpp Players/ClobberPlayerCGTDB.cpp Players/ClobberPlayerDiff.cpp Thermographs/InfinitesimalGame.cpp Thermographs/ASvalue.cpp Thermographs/StopCalculator.cpp Thermographs/Database.cpp -o main