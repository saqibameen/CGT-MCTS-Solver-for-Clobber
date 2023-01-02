#include "Clobber.h"
#include <iostream>
#include <queue>

// default constructor - empty 0x0 clobber board
Clobber::Clobber() {
	this->r = 0;
	this->c = 0;
	this->board_black = 0;
	this->board_white = 0;
	this->track = 0;
	// this->ClearBoard();
}

// constructor - make clobber board of specified dimensions and set to initial checkered state
Clobber::Clobber(int rows, int cols, int track) {
	this->r = rows;
	this->c = cols;
	this->InitBoard();
	this->track = track;
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->InitConnected();
		this->FindLargestSubgame();
	}
}

// constructor - make clobber board of specified dimensions and set to given state
Clobber::Clobber(int rows, int cols, __int128 blacks, __int128 whites, int track) {
	this->r = rows;
	this->c = cols;
	this->board_black = blacks;
	this->board_white = whites;
	this->track = track;
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->InitConnected();
		this->FindLargestSubgame();
	}
}

// constructor - make clobber board given board string
Clobber::Clobber(string board_str, int track) {
  int p1 = board_str.find('c');
  this->r = stoi(board_str.substr(1, p1 - 1));
  int p2 = board_str.find_first_of(".xo", p1);
  this->c = stoi(board_str.substr(p1 + 1, p2 - p1 - 1));
  this->board_black = 0;
  this->board_white = 0;
  int count = 0;
  for (int i = this->r - 1; i >= 0; i--)
    for (int j = 0; j < this->c; j++) {
      switch(board_str[p2 + count]) {
        case 'x':
        this->board_black |= (__int128) 1 << this->Index(i, j);
        break;
        case 'o':
        this->board_white |= (__int128) 1 << this->Index(i, j);
        break;
      }
      count++;
    }
	this->track = track;
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->InitConnected();
		this->FindLargestSubgame();
	}
}

// constructor - copy a clobber board
Clobber::Clobber(const Clobber& c, int track) {
	this->r = c.r;
	this->c = c.c;
	this->board_black = c.board_black;
	this->board_white = c.board_white;
	this->track = track;
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->InitConnected();
		this->FindLargestSubgame();
	}
}

// operator== overload
bool Clobber::operator==(const Clobber& c) const {
	return this->r == c.r && this->c == c.c && this->board_black == c.board_black && this->board_white == c.board_white;
}

// build move tables
int* Clobber::MTTo; int* Clobber::MTFrom; int** Clobber::MTMove; int* Clobber::MTFlip;
__int128* Clobber::MTNorth; __int128* Clobber::MTSouth; __int128* Clobber::MTEast; __int128* Clobber::MTWest;
void Clobber::BuildMoveTables(int r, int c) {
	int move_max = ((r * c - 1) << 2) + 4;
	MTTo = new int[move_max]();
	MTFrom = new int[move_max]();
	MTFlip = new int[move_max]();
	MTMove = new int*[r * c]();
	MTNorth = new __int128[r * c]();
	MTSouth = new __int128[r * c]();
	MTEast = new __int128[r * c]();
	MTWest = new __int128[r * c]();
	for (int i = 0; i < r * c; i++)
		MTMove[i] = new int[r * c]();
	int x1, x2, m1, m2;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			x1 = i * c + j;
			if (i != r - 1) {
				x2 = x1 + c;
				m1 = x1 << 2;
				m2 = (x2 << 2) + 1;
				MTFrom[m1] = x1;
				MTTo[m1] = x2;
				MTMove[x1][x2] = m1;
				MTFlip[m1] = m2;
				MTNorth[x1] = (__int128) 1 << x2;
			}
			if (i != 0) {
				x2 = x1 - c;
				m1 = (x1 << 2) + 1;
				m2 = x2 << 2;
				MTFrom[m1] = x1;
				MTTo[m1] = x2;
				MTMove[x1][x2] = m1;
				MTFlip[m1] = m2;
				MTSouth[x1] = (__int128) 1 << x2;
			}
			if (j != c - 1) {
				x2 = x1 + 1;
				m1 = (x1 << 2) + 2;
				m2 = (x2 << 2) + 3;
				MTFrom[m1] = x1;
				MTTo[m1] = x2;
				MTMove[x1][x2] = m1;
				MTFlip[m1] = m2;
				MTEast[x1] = (__int128) 1 << x2;
			}
			if (j != 0 ) {
				x2 = x1 - 1;
				m1 = (x1 << 2) + 3;
				m2 = (x2 << 2) + 2;
				MTFrom[m1] = x1;
				MTTo[m1] = x2;
				MTMove[x1][x2] = m1;
				MTFlip[m1] = m2;
				MTWest[x1] = (__int128) 1 << x2;
			}
		}
	}
}

// delete move tables
void Clobber::DeleteMoveTables(int r, int c) {
	delete [] MTTo;
	delete [] MTFrom;
	delete [] MTFlip;
	for (int i = 0; i < r * c; i++)
		delete [] MTMove[i];
	delete [] MTMove;
	delete [] MTNorth;
	delete [] MTSouth;
	delete [] MTEast;
	delete [] MTWest;
}

// perform move on clobber board given move string
void Clobber::Move(const string& m_str) {
	int m = this->StrToMove(m_str);
	this->Move(m);
}

// perform move on clobber board given move int
void Clobber::Move(int m) {
	int x1 = GetFrom(m);
	int x2 = GetTo(m);
	switch(this->GetPiece(x1)) {
		case 0:
			this->board_black ^= ((__int128) 1 << x1) + ((__int128) 1 << x2);
			this->board_white ^= (__int128) 1 << x2;
			break;
		case 1:
			this->board_black ^= (__int128) 1 << x2;
			this->board_white ^= ((__int128) 1 << x1) + ((__int128) 1 << x2);
			break;
	}
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->UpdateConnected(x1);
		this->FindLargestSubgame();
	}
}

// reverses a move on clobber board given move string
void Clobber::ReverseMove(const string& m_str){
	int m = this->StrToMove(m_str);
	this->ReverseMove(m);
}

// reverses a move on clobber board given move int
void Clobber::ReverseMove(int m) {
	int x1 = GetFrom(m);
	int x2 = GetTo(m);
	switch(this->GetPiece(x2)) {
		case 0:
			this->board_black ^= ((__int128) 1 << x1) + ((__int128) 1 << x2);
			this->board_white ^= (__int128) 1 << x2;
			break;
		case 1:
			this->board_black ^= (__int128) 1 << x2;
			this->board_white ^= ((__int128) 1 << x1) + ((__int128) 1 << x2);
			break;
	}
	if (this->track) {
		this->occupied = this->board_black | this->board_white;
		this->RevertConnected(x1);
		this->UpdateLargestSubgame(x1);
	}
}

// convert move string to move int
int Clobber::StrToMove(const string& m_str) {
	int p = m_str.find_first_of("0123456789");
	p = m_str.find_first_not_of("0123456789", p);
	int x1 = (stoi(m_str.substr(1, p - 1)) - 1) * this->c + (m_str[0] - 'a');
	int x2 = (stoi(m_str.substr(p + 1)) - 1) * this->c + (m_str[p] - 'a');
	return this->MTMove[x1][x2];
}

// convert move int to move string
string Clobber::MoveToStr(int m) {
	int x1 = MTFrom[m], x2 = MTTo[m];
	string m_str = "";
	m_str += 'a' + this->GetCol(x1);
	m_str += to_string(this->GetRow(x1) + 1);
	m_str += 'a' + this->GetCol(x2);
	m_str += to_string(this->GetRow(x2) + 1);
	return m_str;
}

// get move int given x1 and x2 indices
int Clobber::GetMove(int x1, int x2) {
	return MTMove[x1][x2];
}

// get x1 index given move int
int Clobber::GetFrom(int m) {
	return MTFrom[m];
}

// get x2 index given move int
int Clobber::GetTo(int m) {
	return MTTo[m];
}

// get a new board with a move string applied to the current board
Clobber* Clobber::NextBoard(const string& move) {
	return this->NextBoard(this->StrToMove(move));	
}

// get a new board with a move int applied to the current board
Clobber* Clobber::NextBoard(int move) {
	Clobber* c = new Clobber(*this);
	c->Move(move);
	return c;
}

// get allowable moves of a specified player (black = 0, white = 1)
void Clobber::GetMoves(vector<int>& moves, int player) {
	int x1 = 0;
	__int128 p_board, o_board;
	moves.reserve(this->r * this->c * 4);
	if (player == 0) {
		p_board = this->board_black;
		o_board = this->board_white;
	} else if (player == 1) {
		p_board = this->board_white;
		o_board = this->board_black;
	}
	while (p_board != 0) {
		if (p_board & 1) {
			if (o_board & MTNorth[x1])
				moves.push_back(MTMove[x1][x1 + this->c]);
			if (o_board & MTSouth[x1])
				moves.push_back(MTMove[x1][x1 - this->c]);
			if (o_board & MTEast[x1])
				moves.push_back(MTMove[x1][x1 + 1]);
			if (o_board & MTWest[x1])
				moves.push_back(MTMove[x1][x1 - 1]);
		}
		p_board >>= 1;
		x1++;
	}
}

// update allowable moves given moves of previous board and move taken
void Clobber::UpdateMoves(vector<int>& moves, int move) {
	for (int i = moves.size() - 1; i >= 0; i--)
		if (MTFrom[moves[i]] == MTFrom[move] || MTTo[moves[i]] == MTTo[move])
			moves.erase(moves.begin() + i);
		else
			moves[i] = MTFlip[moves[i]];
	if (MTFrom[move] >= this->r * this->c)
		return;
	int x2 = MTTo[move];
	__int128 p_board;
	switch(this->GetPiece(x2)) {
		case 0:
			p_board = this->board_white;
			break;
		case 1:
			p_board = this->board_black;
			break;
	}
	if (p_board & MTNorth[x2])
		moves.push_back(MTMove[x2 + this->c][x2]);
	if (p_board & MTSouth[x2])
		moves.push_back(MTMove[x2 - this->c][x2]);
	if (p_board & MTEast[x2])
		moves.push_back(MTMove[x2 + 1][x2]);
	if (p_board & MTWest[x2])
		moves.push_back(MTMove[x2 - 1][x2]);
}

void Clobber::FlipMoves(vector<int>& moves) {
	for (int i = 0; i < moves.size(); i++)
		moves[i] = MTFlip[moves[i]];
}

// sets board to initial checkered state
void Clobber::InitBoard() {
	this->ClearBoard();
	for (int i = 0; i < this->r; i++)
		for (int j = 0; j < this->c; j++)
			if ((i + j) % 2 == 0)
				this->board_black |= (__int128) 1 << this->Index(i, j);
			else
				this->board_white |= (__int128) 1 << this->Index(i, j);
}

// clears the clobber board
void Clobber::ClearBoard() {
	this->board_black = 0;
	this->board_white = 0;
}

// initializes east, west, north, and south. Needed for figuring out how many of the orthogonal squares are connected
void Clobber::InitConnected() {
	for (int i = 0; i < this->r; i++) {
		for (int j = 0; j < this->c; j++) {
			if (this->GetOccupied(this->Index(i,j))) {
				if (i < this->r - 1)
					if (this->GetOccupied(this->Index(i + 1, j)))
						this->north |= (__int128) 1 << this->Index(i, j);
				if (i > 0)
					if (this->GetOccupied(this->Index(i - 1, j)))
						this->south |= (__int128) 1 << this->Index(i, j);
				if (j < this->c - 1)
					if (this->GetOccupied(this->Index(i, j + 1)))
						this->east |= (__int128) 1 << this->Index(i, j);
				if (j > 0)
					if (this->GetOccupied(this->Index(i, j - 1)))
						this->west |= (__int128) 1 << this->Index(i, j);
			}
		}
	}
}

// set piece at given row and col (black = 0, white = 1, empty = 2)
void Clobber::SetPiece(int row, int col, int piece) {
	this->SetPiece(this->Index(row, col), piece);
}

// set piece at given index (black = 0, white = 1, empty = 2)
void Clobber::SetPiece(int index, int piece) {
	switch (piece) {
		case 0:
			this->board_black |= (__int128) 1 << index;
			this->board_white &= ~((__int128) 1 << index);
			break;
		case 1:
			this->board_black &= ~((__int128) 1 << index);
			this->board_white |= (__int128) 1 << index;
			break;
		case 2:
			this->board_black &= ~((__int128) 1 << index);
			this->board_white &= ~((__int128) 1 << index);
			break;
	}
}

// get number of rows of board
int Clobber::Rows() {
	return this->r;
}

// get number of cols of board
int Clobber::Cols() {
	return this->c;
}

// get index of given row and col
int Clobber::Index(int row, int col) {
	return row * this->c + col;
}

// get row of given index
int Clobber::GetRow(int index) {
	return index / this->c;
}

// get col of given index
int Clobber::GetCol(int index) {
	return index % this->c;
}

// get piece at given row and col (black = 0, white = 1, empty = 2)
int Clobber::GetPiece(int row, int col) {
	return this->GetPiece(this->Index(row, col));
}

// get piece at given index (black = 0, white = 1, empty = 2)
int Clobber::GetPiece(int index) {
	if ((this->board_black >> index) & 1)
		return 0;
	else if ((this->board_white >> index) & 1)
		return 1;
	else
		return 2;
}

// get the board given a color (black = 0, white = 1)
__int128 Clobber::GetBoard(int player) {
	if (player == 0)
		return this->board_black;
	else if (player == 1)
		return this->board_white;
	else
		return 0;
}

// get mask of each subgame
void Clobber::GetSubgames(vector<__int128>& sgmasks) {
	sgmasks.reserve(this->r * this->c / 2);
	queue<int> frontier;
	__int128 v = 0, vp;
	__int128 pieces = this->board_black | this->board_white;
	__int128 pieceshift = pieces;
	int x = 0, f;
	while (pieceshift != 0) {
		if ((pieceshift & 1) && !((v >> x) & 1)) {
			vp = 0;
			frontier.push(x);
			while (!frontier.empty()) {
				f = frontier.front();
				if ((vp >> f) & 1) {
					frontier.pop();
					continue;
				}
				vp |= (__int128) 1 << f;
				if ((pieces & MTNorth[f]) && !(vp & MTNorth[f]))
					frontier.push(f + this->c);
				if ((pieces & MTSouth[f]) && !(vp & MTSouth[f]))
					frontier.push(f - this->c);
				if ((pieces & MTEast[f]) && !(vp & MTEast[f]))
					frontier.push(f + 1);
				if ((pieces & MTWest[f]) && !(vp & MTWest[f]))
					frontier.push(f - 1);
			}
			if ((this->board_black & vp) && (this->board_white & vp))
				sgmasks.push_back(vp);
			v |= vp;
		}
		pieceshift >>= 1;
		x++;
	}
}

// Returns true if there's only one player left in the subgame
bool Clobber::DeadSubgame(__int128 sgmask) {
	if ((this->board_black & sgmask) == 0 || (this->board_white & sgmask) == 0)
		return true;
	return false;
}

// returns 1 if the square at index is occuppied
int Clobber::GetOccupied(int index) {
	return (this->occupied >> index) & 1;
}

// returns the number of pieces orthogonal to the piece at index
int Clobber::GetConnected(int index) {
	return ((this->north >> index) & 1) + ((this->south >> index) & 1) +
					((this->east >> index) & 1) + ((this->west >> index) & 1);
}

// returns the size of the largest subgame on the board
int Clobber::GetLargestSubgame() {
	return this->largest;
}

// returns 0 if there's only one subgame in the board, and 1 if there's more than one
int Clobber::MoreThanOneSubgame() {
	return this->subgames;
}

// checks if there are any empty columns in the game
int Clobber::CheckEmpty() {
	int empty_col;
	int empty_rows[this->r];
	for (int i = 0; i < this->r; i++)
		empty_rows[i] = 0;
	for (int i=0; i < this->c; ++i){
		empty_col = 1;
		for (int j=0; j < this->r; ++j){
			if (this->GetPiece(j,i) != 2){
				empty_col = 0;
				empty_rows[j] = 1;
			}
		}
		if (empty_col)
			return 1;
	}
	for (int j=0; j < this->r; ++j)
		if (empty_rows[j] == 0)
			return 1;
	return 0;
}

// Sets the track to 0 or 1
void Clobber::SetTrack(int track) {
	if (track == this->track)
		return;
	else if (track == 1 && this->track == 0) {
		this->occupied = this->board_black | this->board_white;
		this->InitConnected();
		this->FindLargestSubgame();
		this->track = 1;
	} else if (track == 0 && this->track == 1)
		this->track = 0;
}

// Finds the largest subgame on the board and sets the variable largest
void Clobber::FindLargestSubgame() {
	__int128 visited = 0;
	int overall_largest = 0;
	int first = 0;
	for (int i = 0; i < this->r; i++) {
		for (int j = 0; j < this->c; j++) {
			vector<int> connected_to;
			int indx = this->Index(i,j);
			if (this->GetOccupied(indx) && ((visited >> indx) & 1)==0 ) { // If the square is occupied and it hasn't been visited
				first += 1;
				connected_to.push_back(this->Index(i,j));
				int temp_largest = 0;
				visited |= (__int128) 1 << indx;
				while(!connected_to.empty()) {
					temp_largest += 1;
					int temp_indx = connected_to.back();
					connected_to.pop_back();
					int temp_col = this->GetCol(temp_indx), temp_row = this->GetRow(temp_indx);
					if (temp_col < this->c - 1) {
						int east_index = this->Index(temp_row, temp_col+1);
						if (this->GetOccupied(east_index) && ((visited >> east_index) & 1) == 0) {
							visited |= (__int128) 1 << east_index;
							if (this->GetConnected(east_index) > 1)
								connected_to.push_back(east_index);
							else
								temp_largest += 1;
						}
					}
					if (temp_col > 0) {
						int west_index = this->Index(temp_row, temp_col-1);
						if (this->GetOccupied(west_index) && ((visited >> west_index) & 1) == 0) {
							visited |= (__int128) 1 << west_index;
							if (this->GetConnected(west_index) > 1)
								connected_to.push_back(west_index);
							else
								temp_largest += 1;
						}
					}
					if (temp_row < this->r - 1) {
						int north_index = this->Index(temp_row+1, temp_col);
						if (this->GetOccupied(north_index) && ((visited >> north_index) & 1) == 0) {
							visited |= (__int128) 1 << north_index;
							if (this->GetConnected(north_index) > 1)
								connected_to.push_back(north_index);
							else
								temp_largest += 1;
						}
					}
					if (temp_row > 0) {
						int south_index = this->Index(temp_row-1, temp_col);
						if (this->GetOccupied(south_index) && ((visited >> south_index) & 1) == 0) {
							visited |= (__int128) 1 << south_index;
							if (this->GetConnected(south_index) > 1)
								connected_to.push_back(south_index);
							else
								temp_largest += 1;
						}
					}
				}
				if (temp_largest > overall_largest)
					overall_largest = temp_largest;
				if (first == 1){
					if (visited == this->occupied){
						this->subgames = 0;
					}else if (visited != this->occupied){
						this->subgames = 1;
					}
				}
			}
			if (visited == this->occupied) break;
		}
		if (visited == this->occupied) break;
	}

	this->largest = overall_largest;
	// cout << "Overall largest: " << overall_largest << endl;
}


// updates the largest subgame after a move. If revert is 1, then the move is reverted
void Clobber::UpdateLargestSubgame(int indx) {
	__int128 visited = 0;
	vector<int> connected_to;
	connected_to.push_back(indx);
	int temp_largest = 0;
	visited |= (__int128) 1 << indx;
	while(!connected_to.empty()) {
		temp_largest += 1;
		int temp_indx = connected_to.back();
		connected_to.pop_back();
		int temp_col = this->GetCol(temp_indx), temp_row = this->GetRow(temp_indx);
		if (temp_col < this->c - 1) {
			int east_index = this->Index(temp_row, temp_col+1);
			if (this->GetOccupied(east_index) && ((visited >> east_index) & 1) == 0) {
				visited |= (__int128) 1 << east_index;
				if (this->GetConnected(east_index) > 1)
					connected_to.push_back(east_index);
				else
					temp_largest += 1;
			}
		}
		if (temp_col > 0) {
			int west_index = this->Index(temp_row, temp_col-1);
			if (this->GetOccupied(west_index) && ((visited >> west_index) & 1) == 0) {
				visited |= (__int128) 1 << west_index;
				if (this->GetConnected(west_index) > 1)
					connected_to.push_back(west_index);
				else
					temp_largest += 1;
			}
		}
		if (temp_row < this->r - 1) {
			int north_index = this->Index(temp_row+1, temp_col);
			if (this->GetOccupied(north_index) && ((visited >> north_index) & 1) == 0) {
				visited |= (__int128) 1 << north_index;
				if (this->GetConnected(north_index) > 1)
					connected_to.push_back(north_index);
				else
					temp_largest += 1;
			}
		}
		if (temp_row > 0) {
			int south_index = this->Index(temp_row-1, temp_col);
			if (this->GetOccupied(south_index) && ((visited >> south_index) & 1) == 0) {
				visited |= (__int128) 1 << south_index;
				if (this->GetConnected(south_index) > 1)
					connected_to.push_back(south_index);
				else
					temp_largest += 1;
			}
		}
	}
	if (temp_largest > this->largest)
		this->largest = temp_largest;
	if (visited == this->occupied)
		this->subgames = 0;
}


// Updates the connected squares after a move
void Clobber::UpdateConnected(int indx) {
	int temp_row = indx / this->c, temp_col = indx % this->c;
	if (temp_row < this->r - 1) {
		int north_index = this->Index(temp_row + 1, temp_col);
		if (this->GetOccupied(north_index)) {
			this->south &= ~((__int128) 1 << north_index);
			this->north &= ~((__int128) 1 << indx);
		}
	}
	if (temp_row > 0) {
		int south_index = this->Index(temp_row - 1, temp_col);
		if (this->GetOccupied(south_index)) {
			this->north &= ~((__int128) 1 << south_index);
			this->south &= ~((__int128) 1 << indx);
		}
	}
	if (temp_col < this->c - 1) {
		int east_index = this->Index(temp_row, temp_col + 1);
		if (this->GetOccupied(east_index)) {
			this->west &= ~((__int128) 1 << east_index);
			this->east &= ~((__int128) 1 << indx);
		}
	}
	if (temp_col > 0) {
		int west_index = this->Index(temp_row, temp_col -1);
		if (this->GetOccupied(west_index)) {
			this->east &= ~((__int128) 1 << this->Index(temp_row, temp_col - 1));
			this->west &= ~((__int128) 1 << indx);
		}
	}
}


// Reverts the connected squares after a move
void Clobber::RevertConnected(int indx) {
	int temp_row = indx / this->c, temp_col = indx % this->c;
	if (temp_row < this->r - 1) {
		int north_index = this->Index(temp_row + 1, temp_col);
		if (this->GetOccupied(north_index)) {
			this->south |= (__int128) 1 << north_index;
			this->north |= (__int128) 1 << indx;
		}
	}
	if (temp_row > 0) {
		int south_index = this->Index(temp_row - 1, temp_col);
		if (this->GetOccupied(south_index)) {
			this->north |= (__int128) 1 << south_index;
			this->south |= (__int128) 1 << indx;
		}
	}
	if (temp_col < this->c - 1) {
		int east_index = this->Index(temp_row, temp_col + 1);
		if (this->GetOccupied(east_index)) {
			this->west |= (__int128) 1 << east_index;
			this->east |= (__int128) 1 << indx;
		}
	}
	if (temp_col > 0) {
		int west_index = this->Index(temp_row, temp_col -1);
		if (this->GetOccupied(west_index)) {
			this->east |= (__int128) 1 << west_index;
			this->west |= (__int128) 1 << indx;
		}
	}
}



string Clobber::GetSubgameString(__int128 sgmask) {
	int rlow=this->r-1, rup=0, collow=this->c-1, colup=0;
	for (int i = this->r - 1; i >= 0; i--) {
		for (int j = 0; j < this->c; j++) {
			if ((sgmask >> this->Index(i, j)) & 1) {
				if (i > rup)
					rup = i;
				if (i < rlow)
					rlow = i;
				if (j > colup)
					colup = j;
				if (j < collow)
					collow = j;
			}
		}
	}
	string board_str = "r" + to_string(rup - rlow + 1) + "c" + to_string(colup - collow +1);
	for (int i = rup; i >= rlow; i--){
		for (int j = collow; j <= colup; j++){
			switch(this->GetPiece(i, j)) {
				case 0:
					board_str += 'x';
					break;
				case 1:
					board_str += 'o';
					break;
				case 2:
					board_str += '.';
					break;
			}
		}
	}
	return board_str;
}

// count the number of active subgames on the board
int Clobber::CountSubgames() {
  vector<__int128> sgmasks;
  this->GetSubgames(sgmasks);
  return sgmasks.size();
}

// get a copy of the current board with a subgame mask applied
Clobber* Clobber::GetSubgame(__int128 sgmask) {
  Clobber* c = new Clobber(*this);
  c->board_black &= sgmask;
  c->board_white &= sgmask;
  return c;
}

Clobber Clobber::FernandoGetSubgame(__int128 sgmask) {
  Clobber c = Clobber(this->r, this->c, this->board_black, this->board_white);
  c.board_black &= sgmask;
  c.board_white &= sgmask;
  return c;
}

// use subgame masks to remove dead pieces from the board
void Clobber::FilterBoard(vector<__int128>& sgmasks) {
	__int128 live_mask = 0;
	for (int i = 0; i < sgmasks.size(); i++)
		live_mask |= sgmasks[i];
	this->board_black &= live_mask;
	this->board_white &= live_mask;
}

// remove a subgame from the current board
void Clobber::RemoveSubgame(__int128 sgmask) {
	this->board_black &= ~sgmask;
	this->board_white &= ~sgmask;
}

// compute difference in active pieces
int Clobber::ActiveDifferential(int player) {
	queue<int> frontier;
	__int128 v = 0, vp = 0;
	__int128 b_board = this->board_black, w_board = this->board_white;
	int f_r, f_c, x = 0;
	bool b_exists, w_exists;
	int val = 0, sval;
	while(b_board != 0 || w_board != 0) {
		if (!((v >> x) & 1)) {
			b_exists = (b_board & 1);
			w_exists = (w_board & 1);
			if (b_exists || w_exists) {
				sval = 0;
				frontier.push(x);
				vp = 0;
				while (!frontier.empty()) {
					if ((vp >> frontier.front()) & 1) {
            frontier.pop();
            continue;
          }
					vp |= (__int128) 1 << frontier.front();
					if (this->GetPiece(frontier.front()) == 0) {
						b_exists = true;
						sval++;
					} else {
						w_exists = true;
						sval--;
					}
					f_r = this->GetRow(frontier.front());
					f_c = this->GetCol(frontier.front());
					if (f_r != this->r - 1)
						if (this->GetPiece(frontier.front() + this->c) != 2 && !((vp >> (frontier.front() + this->c)) & 1))
							frontier.push(frontier.front() + this->c);
					if (f_r != 0)
						if (this->GetPiece(frontier.front() - this->c) != 2 && !((vp >> (frontier.front() - this->c)) & 1))
							frontier.push(frontier.front() - this->c);
					if (f_c != this->c - 1)
						if (this->GetPiece(frontier.front() + 1) != 2 && !((vp >> (frontier.front() + 1)) & 1))
							frontier.push(frontier.front() + 1);
					if (f_c != 0)
						if (this->GetPiece(frontier.front() - 1) != 2 && !((vp >> (frontier.front() - 1)) & 1))
							frontier.push(frontier.front() - 1);
					frontier.pop();
				}
				if (b_exists && w_exists)
					val += sval;
				v |= vp;
			}
		}
		x++;
		b_board >>= 1;
		w_board >>= 1;
	}
	if (player == 0)
		return val;
	else
		return -val;
}

// get string of current board
string Clobber::ToString() {
	string board_str = "r" + to_string(this->r) + "c" + to_string(this->c);
	for (int i = this->r - 1; i >= 0; i--)
		for (int j = 0; j < this->c; j++)
			switch(this->GetPiece(i, j)) {
				case 0:
					board_str += 'x';
					break;
				case 1:
					board_str += 'o';
					break;
				case 2:
					board_str += '.';
					break;
			}
	return board_str;
}

// print the clobber board to stdout
void Clobber::PrintBoard() {
	cout << "   ";
	for (int i = 0; i < this->c; i++)
		cout << (char)('a' + i) << ' ';
	cout << '\n';
	for (int i = this->r - 1; i >= 0; i--) {
		printf("%2d", i + 1);
		cout << ' ';
		for (int j = 0; j < this->c; j++)
			switch(this->GetPiece(i, j)) {
				case 0:
					cout << "● ";
					break;
				case 1:
					cout << "○ ";
					break;
				default:
					cout << "· ";
			}
		cout << '\n';
	}
}