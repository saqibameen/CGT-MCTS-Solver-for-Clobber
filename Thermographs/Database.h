#ifndef DATABASE_H
#define DATABASE_H

#include "ASvalue.h"
#include "StopCalculator.h"
#include "InfGamesDefinitions.h"
#include "InfinitesimalGame.h"
#include "DatabaseEntry.h"
#include "../Clobber.h"
#include "../Solvers/Search/HashEntry.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>

// Constant definitions
const int DEFAULT_DATABASE_SIZE = 32452867; //15485863; //1280023; //15485863; //122949823;//49979687;// 122949823;//86028121;//
//const string SOURCE_PATH = "/home/jfernando/Documents/CGT_Final_Project/clobber/Thermographs/";

// Type definitions
typedef unsigned long long int ulli;

using namespace std;
class Database {
private:
    ReferenceGames rf;
    StopCalculator st;
public:
    DatabaseEntry** DataTable;
    int size;
    Database();
    Database(int sz);
    ~Database();
    void LoadDatabase(int summary);

    /// Functions for Storing and retrieving values ///
    void RecordDatabaseEntry(int flag, ASvalue exact, ASvalue lower, ASvalue upper, string brd, int save);
    int LookUpKey(string board);
    ASvalue RetrieveExactValue(string board);
    ASvalue RetrieveUpperBound(string board);
    ASvalue RetrieveLowerBound(string board);
    int RetrieveFlag(string board);
    DatabaseEntry * RetrieveEntry(string board);
    string GetBoard(string board, int version);

    /// Functions for building the database ///
    void ComputeDatabase(int size, int save);
    void StoreBoardValue(string board, int save);
    void BuildCheckerString(string board, vector<string>& boards, int whiteChecker, int size);
    void BuildBoard(int cols, int usedcols, int empty, string board, vector<string>& boards, int index,
                    string checkers);
    void BuildBoardsSizeN(int size, vector<string>& boards);
    void SaveEntry(int flag, ASvalue exact, ASvalue lower, ASvalue upper, string brd);

    /// Hash function ///
    ulli djb2(string board);

    /// Utility functions for inverse and symmetry
    string Inverse(string board);
    string Symmetry(string board);

    /// Database summary
    void AddToSummary(ASvalue exact_value, int print, string board);

    int collisions=0;
    int stored=0;
    int maxsize=0;
    int minsize=100000;
};

#endif //DATABASE_H
