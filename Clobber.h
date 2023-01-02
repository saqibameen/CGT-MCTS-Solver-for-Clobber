#ifndef CLOBBER_H
#define CLOBBER_H

#include <string>
#include <vector>

using namespace std;

class Clobber {

private:
    int r;
    int c;
    int largest=0;
    int subgames=0; // 0 if only 1 subgame on the board, 1 if more than one

    __int128 board_black;
    __int128 board_white;
    
    __int128 occupied=0;
    __int128 east=0;
    __int128 west=0;
    __int128 north=0;
    __int128 south=0;
public:

    int track; // Specifies whether to keep track of subgames or not

    static int* MTTo;
    static int* MTFrom;
    static int** MTMove;
    static int* MTFlip;
    static __int128* MTNorth;
    static __int128* MTSouth;
    static __int128* MTEast;
    static __int128* MTWest;

    Clobber();
    Clobber(int rows, int cols, int track=0);
    Clobber(int rows, int cols, __int128 blacks, __int128 whites, int track=0);
    Clobber(string board_str, int track=0);
    Clobber(const Clobber& c, int track=0);

    bool operator==(const Clobber& c) const;

    static void BuildMoveTables(int r, int c);
    static void DeleteMoveTables(int r, int c);
    void Move(const string& m_str);
    void Move(int m);
    void ReverseMove(const string& m_str);
    void ReverseMove(int m);
    int StrToMove(const string& m_str);
    string MoveToStr(int m);
    int GetMove(int x1, int x2);
    int GetFrom(int m);
    int GetTo(int m);

    Clobber* NextBoard(const string& move);
    Clobber* NextBoard(int move);
    void GetMoves(vector<int>& moves, int player);
    void UpdateMoves(vector<int>& moves, int move);
    void FlipMoves(vector<int>& moves);

    void InitBoard();
    void ClearBoard();
    void InitConnected();
    void UpdateLargestSubgame(int indx);
    void UpdateConnected(int indx);
    void RevertConnected(int indx);
    void SetPiece(int row, int col, int piece);
    void SetPiece(int index, int piece);

    int Rows();
    int Cols();
    int Index(int row, int col);
    int GetRow(int index);
    int GetCol(int index);
    int GetPiece(int row, int col);
    int GetPiece(int index);
    __int128 GetBoard(int player);

    void GetSubgames(vector<__int128>& sgmasks);
    int CountSubgames();
    Clobber* GetSubgame(__int128 sgmask);
    void FilterBoard(vector<__int128>& sgmasks);
    void RemoveSubgame(__int128 sgmask);
    int GetOccupied(int index);
    int GetConnected(int index);
    int GetLargestSubgame();
    int MoreThanOneSubgame();
    int CheckEmpty();
    void SetTrack(int track);

    string GetSubgameString(__int128 sgmask);
    Clobber FernandoGetSubgame(__int128 sgmask);
    bool DeadSubgame(__int128 sgmask);
    void FindLargestSubgame();

    int ActiveDifferential(int player);

    string ToString();
    void PrintBoard();
};

#endif