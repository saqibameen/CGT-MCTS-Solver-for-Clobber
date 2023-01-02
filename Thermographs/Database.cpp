#include "Database.h"

/// Functions for initializing the database ///
Database::Database() {
  this->DataTable = new DatabaseEntry*[DEFAULT_DATABASE_SIZE];
  this->size = DEFAULT_DATABASE_SIZE;
}

Database::Database(int sz) {
  this->DataTable = new DatabaseEntry*[sz];
  this->size = sz;
}

Database::~Database() {
  for (int i = 0; i < this->size; ++i) {
    DatabaseEntry* current = this->DataTable[i];
    while (current != NULL) {
      DatabaseEntry* previous = current;
      current = current->GetNext();
      delete previous;
    }
    this->DataTable[i] = NULL;
  }
  delete [] DataTable;
}

// This function writes the database entry onto a txt file
void Database::SaveEntry(int flag, ASvalue exact, ASvalue lower, ASvalue upper, string brd) {
  ofstream outfile;
  outfile.open("data.txt", ios::app);
  if (outfile.is_open()) {
    outfile << " " << flag << " ";
    outfile << exact.u << " " << exact.u2 << " " << exact.u3 << " " << exact.star << " ";
    outfile << lower.u << " " << lower.u2 << " " << lower.u3 << " " << lower.star << " ";
    outfile << upper.u << " " << upper.u2 << " " << upper.u3 << " " << upper.star << " ";
    outfile << brd << " " << "\n";
    outfile.close();
  }
}

void Database::LoadDatabase(int summary) {
  string line;
  ifstream inputfile ("data.txt");
  if (inputfile.is_open()){
    while(getline(inputfile, line)){
      ulli p = line.find_first_of(" ");
      int flag = line[p+1]-'0';
      p += 2;
      int u[4];
      ulli p2;
      for (int i = 0; i < 4; i++){
        p2 = line.find_first_of(" ", p+1);
        string frag = line.substr(p+1,p2-p-1);
        u[i] = stoi(line.substr(p+1,p2-p-1));
        p = p2;
      }
      ASvalue exact = ASvalue(u[0], u[1], u[2], u[3]);
      for (int i = 0; i < 4; i++){
        p2 = line.find_first_of(" ", p+1);
        string frag = line.substr(p+1,p2-p-1);
        u[i] = stoi(line.substr(p+1,p2-p-1));
        p = p2;
      }
      ASvalue lower = ASvalue(u[0], u[1], u[2], u[3]);
      for (int i = 0; i < 4; i++){
        p2 = line.find_first_of(" ", p+1);
        string frag = line.substr(p+1,p2-p-1);
        u[i] = stoi(line.substr(p+1,p2-p-1));
        p = p2;
      }
      ASvalue upper = ASvalue(u[0], u[1], u[2], u[3]);
      p2 = line.find_first_of(" ", p+1);
      string frag = line.substr(p+1,p2-p-1);
      string board = line.substr(p+1, p2-p-1);
      int dbsize = 0;
      for (int i = 0; i < board.size(); ++i)
        if (board[i] == 'x' || board[i] == 'o')
          dbsize += 1;
      this->maxsize = dbsize > this->maxsize? dbsize : this->maxsize;
      this->minsize = dbsize < this->minsize? dbsize : this->minsize;
      this->RecordDatabaseEntry(flag, exact, lower, upper, board, 0);
      if (summary)
        if (flag == 1 || (flag==0 && upper == rf.upups.value && lower == rf.dpdps.value))
          this->AddToSummary(exact, 0, board); 
    }
    inputfile.close();
  }
}

/// Functions for Retrieving and storing entries from the database ////
// Returns one of the 4 different versions of the key: 0 for original, 1 for symmetry, 2 for inverse, and 3 for
// symmetry inverse
string Database::GetBoard(string board, int version) {
  if (version == 0)
    return board;
  else if (version == 1)
    return this->Symmetry(board);
  else if (version == 2)
    return this->Inverse(board);
  else if (version == 3)
    return this->Inverse(this->Symmetry(board));
  cout << "Wrong version!" << endl;
  return "x";
}

void Database::RecordDatabaseEntry(int flag, ASvalue exact, ASvalue lower, ASvalue upper, string brd, int save) {
  ulli key = this->djb2(brd);
  DatabaseEntry* current = this->DataTable[key % this->size];
  DatabaseEntry* previous = NULL;
  while (current != NULL) {
    this->collisions += 1;
    previous = current;
    current = current->GetNext();
  }
  if (previous == NULL)
    this->DataTable[key % this->size] = new DatabaseEntry(key, flag, exact, lower, upper, brd);
  else
    previous->SetNext(new DatabaseEntry(key, flag, exact, lower, upper, brd));
  this->stored += 1;
  if (save)
    this->SaveEntry(flag, exact, lower, upper, brd);
}

// Returns 1 if the key is in the table and the stored key matches the provided key
int Database::LookUpKey(string board) {
  int versions = 3;
  for (int i = 0; i <= versions; i++) {
    string temp_board = this->GetBoard(board, i);
    ulli key = this->djb2(temp_board);
    DatabaseEntry* current = this->DataTable[key % this->size];
    while (current != NULL) {
      if (current->key == key && current->board == temp_board)
        return 1;
      else
        current = current->GetNext();
    }
  }
  return 0;
}

// Returns the exact value of the corresponding board;
ASvalue Database::RetrieveExactValue(string board) {
  int versions = 3;
  string temp_board;
  ulli key;
  for (int i = 0; i <= versions; i++) {
    temp_board = this->GetBoard(board, i);
    key = this->djb2(temp_board);
    DatabaseEntry* current = this->DataTable[key % this->size];
    while (current != NULL) {
      if (current->key == key && current->board == temp_board)
        if (i >= 2)
          return current->exact_value.Inverse();
        else
          return current->exact_value;
      else
        current = current->GetNext();
    }
  }
  return ASvalue(0,0,0,0);
}

// Returns the lower bound of the corresponding board
ASvalue Database::RetrieveLowerBound(string board) {
  int versions = 3;
  string temp_board;
  ulli key;
  for (int i = 0; i <= versions; i++) {
    temp_board = this->GetBoard(board, i);
    key = this->djb2(temp_board);
    DatabaseEntry* current = this->DataTable[key % this->size];
    while (current != NULL) {
      if (current->key == key && current->board == temp_board)
        if (i >= 2)
          return current->lower_bound.Inverse();
        else
          return current->lower_bound;
      else
        current = current->GetNext();
    }
  }
  return ASvalue(0,0,0,0);
}

// Returns the upper bound of the corresponding board
ASvalue Database::RetrieveUpperBound(string board) {
  int versions = 3;
  string temp_board;
  ulli key;
  for (int i = 0; i <= versions; i++) {
    temp_board = this->GetBoard(board, i);
    key = this->djb2(temp_board);
    DatabaseEntry* current = this->DataTable[key % this->size];
    while (current != NULL) {
      if (current->key == key && current->board == temp_board)
        if (i >= 2)
          return current->upper_bound.Inverse();
        else
          return current->upper_bound;
      else
        current = current->GetNext();
    }
  }
  return ASvalue(0,0,0,0);
}

// Returns the flag of the stored key
int Database::RetrieveFlag(string board) {
  int versions = 3;
  string temp_board;
  ulli key;
  for (int i = 0; i <= versions; i++) {
    temp_board = this->GetBoard(board, i);
    key = this->djb2(temp_board);
    DatabaseEntry *current = this->DataTable[key % this->size];
    while (current != NULL) {
      if (current->key == key && current->board == temp_board)
        return current->flag;
      else
        current = current->GetNext();
    }
  }
  return 0;
}

// Returns the entry corresponding to the board, if not found, it returns NULL
DatabaseEntry* Database::RetrieveEntry(string board) {
  int versions =3;
  string temp_board;
  ulli key;
  for (int i = 0; i <= versions; i++) {
    temp_board = this->GetBoard(board, i);
    key = this->djb2(temp_board);
    DatabaseEntry* current = this->DataTable[key % this->size];
    while (current != NULL)
      if (current->key == key && current->board == temp_board)
        return current;
      else
        current = current->GetNext();
  }
  return NULL;
}

/// Functions for building the database ///
// Computes the values of boards of size rows x [lb,ub] and stores them in the database
void Database::ComputeDatabase(int size, int save) {
  Clobber c;
  int max_rows = (size /2) + (size % 2);
  int max_cols = size + 1 - max_rows;
  int dims = max_rows * max_cols;
  if (size == 8)
    dims = 16;
  this->maxsize = dims > this->maxsize? dims : this->maxsize;
  this->minsize = dims < this->minsize? dims : this->minsize;

  vector<string> boards;
  this->BuildBoardsSizeN(size, boards);
  int i = 0;
  for (int b = 0; b < boards.size(); ++b) {
    string f = boards[b];
    vector<int> moves;
    c = Clobber(boards[b]);
    c.GetMoves(moves, 0);
    vector<__int128> sgmasks;
    c.GetSubgames(sgmasks);
    if (!moves.empty() && !c.CheckEmpty() && sgmasks.size() == 1) {
      if (!c.FernandoGetSubgame(sgmasks[0]).CheckEmpty() && !this->LookUpKey(c.ToString())) {
        this->StoreBoardValue(boards[b], save);
        ++i;
      }
    }
  }
}

// This function finds the left and right stops of the game compared to the reference infinitesimals:
// up, down, up^3, star, and so on...
// Then it stores the exact value of the game if it was found. Otherwise, it stores the lower and upper bound of the
// game.
void Database::StoreBoardValue(string board, int save) {
  Clobber g = Clobber(board);
  int game_leftstop=0, game_rightstop=0;

  // Checking if the game is zero...
  int zero_leftstop = this->st.GetLeftStop(&g, &this->rf.zero);
  int zero_rightstop = this->st.GetRightStop(&g, &this->rf.zero);
  if (zero_leftstop == zero_rightstop) { // The game is zero
    this->RecordDatabaseEntry(1, this->rf.zero.value, this->rf.zero.value, this->rf.zero.value, board, save);
    return;
  }

  else if (zero_leftstop == 1 && zero_rightstop == -1){ // The game is in the star scale

    // Checking if the game is star...
    int star_leftstop = this->st.GetLeftStop(&g, &this->rf.star);
    int star_rightstop = this->st.GetRightStop(&g, &this->rf.star);
    if (star_leftstop == 1 && star_rightstop == -1) { // If this happens the game is a switch
      //this->RecordDatabaseEntry(0, this->rf.unkn.value, this->rf.unkn.value, this->rf.unkn.value, board, save);
      this->RecordDatabaseEntry(0, this->rf.unkn.value, this->rf.dpdps.value, this->rf.upups.value, board, save);
      return;
    }
    else if (star_leftstop == star_rightstop) { // The game is star
      this->RecordDatabaseEntry(1, this->rf.star.value, this->rf.zero.value, this->rf.zero.value, board, save);
      return;
    }

    else { // The game is between up + up2 + star and down + down2 + star
      vector<InfinitesimalGame> games;
      if (star_leftstop == 0 && star_rightstop == -1) // The game is < 0, so we compare it to positive games
        games = this->rf.PositiveStarGames;
      else                                            // The game is > 0, so we compare it to negative games
        games = this->rf.NegativeStarGames;

      ASvalue previous = this->rf.star.value;
      for (int i = 0; i < games.size(); ++i){
        game_leftstop = this->st.GetLeftStop(&g, &games[i]);
        game_rightstop = this->st.GetRightStop(&g, &games[i]);
        if (game_leftstop == game_rightstop) {              // The game is the inverse of the comparison game
          this->RecordDatabaseEntry(1, games[i].value.Inverse(), this->rf.zero.value, this->rf.zero.value, board, save);
          return;
        }
        else if (game_leftstop == games[i].LeftStop && game_rightstop == games[i].RightStop) {
          if (star_leftstop == 0 && star_rightstop == -1) { // -current < g < -previous if g < 0
            this->RecordDatabaseEntry(0, this->rf.zero.value, games[i].value.Inverse(), previous.Inverse(), board, save);
            return;
          }
          else {                                            // -previous < g < -current if g > 0
            this->RecordDatabaseEntry(0, this->rf.zero.value, previous.Inverse(), games[i].value.Inverse(), board, save);
            return;
          }
        }
        previous = games[i].value;
      }
      if (star_leftstop == 0 && star_rightstop == -1) {
        this->RecordDatabaseEntry(0, this->rf.zero.value, this->rf.unkn.value, games.back().value.Inverse(), board, save);
        return;
      }
      else {
        this->RecordDatabaseEntry(0, this->rf.zero.value, games.back().value.Inverse(), this->rf.unkn.value, board, save);
        return;
      }
    }
  }
    // At this point the game is not a star game or zero
  else{
    vector<InfinitesimalGame> games;
    if (zero_leftstop == 0 && zero_rightstop == -1)
      games = this->rf.PositiveGames;
    else
      games = this->rf.NegativeGames;
    ASvalue previous = this->rf.zero.value;
    for (int i = 0; i < games.size(); ++i){
      InfinitesimalGame infg = games[i];
      game_leftstop = this->st.GetLeftStop(&g, &games[i]);
      game_rightstop = this->st.GetRightStop(&g, &games[i]);
      if (game_leftstop == game_rightstop) {                // The game is the inverse of the comparison game
        this->RecordDatabaseEntry(1, games[i].value.Inverse(), this->rf.zero.value, this->rf.zero.value, board, save);
        return;
      }
      else if (game_leftstop == games[i].LeftStop && game_rightstop == games[i].RightStop) { // -previous < g < -current
        if (zero_leftstop == 0 && zero_rightstop == -1) {   // -current < g < -previous if g < 0
          this->RecordDatabaseEntry(0, this->rf.zero.value, previous.Inverse(), games[i].value.Inverse(), board, save);
          return;
        }
        else {                                              // -previous < g < -current if g > 0
          this->RecordDatabaseEntry(0, this->rf.zero.value, games[i].value.Inverse(), previous.Inverse(), board, save);
          return;
        }
      }
      previous = games[i].value;
    }
    if (zero_leftstop == 0 && zero_rightstop == -1) {
      this->RecordDatabaseEntry(0, this->rf.zero.value, this->rf.unkn.value, games.back().value.Inverse(), board, save);
      return;
    }
    else {
      this->RecordDatabaseEntry(0, this->rf.zero.value, games.back().value.Inverse(), this->rf.unkn.value, board, save);
      return;
    }
  }
}

// Builds as string of black and white checkers with at least one white
void Database::BuildCheckerString(string board, vector<string> &boards, int whiteChecker, int size) {
  if (size == 0)
    return;
  else if (size == 1) {
    if (whiteChecker != 0)
      boards.push_back(board + "x");
    boards.push_back(board + "o");
  }
  else {
    this->BuildCheckerString(board + "x", boards, whiteChecker, size -1);
    this->BuildCheckerString(board + "o", boards, 1,            size -1);
  }
}


// Builds all the board of a given size and stores them in a vector of strings
void Database::BuildBoard(int cols, int usedcols, int empty, string board, vector<string> &boards, int index,
                          string checkers) {
  if (index >= checkers.size()) {
    if (empty > cols)
      return;
    if (empty != 0)
      for(int j = 0; j < empty; ++j)
        board += ".";
    boards.push_back(board);
  }
  else {
    string bempty = board;
    int temp_empty = empty;
    for (int j = 0; usedcols <= cols-1 && j < temp_empty; j++){
      bempty += ".";
      temp_empty -= 1;
      this->BuildBoard(cols, usedcols + j + 1, temp_empty, bempty, boards, index, checkers);
    }
    if (usedcols == cols -1)
      usedcols = 0;
    board += checkers[index];
    this->BuildBoard(cols, usedcols, empty, board, boards, index+1, checkers);
  }
}

void Database::BuildBoardsSizeN(int size, vector<string> &boards) {
  vector<string> checkers;
  this->BuildCheckerString("x", checkers, 0, size-1);
  int max_rows = (size / 2) + (size % 2);
  for (int rows = 1; rows <= max_rows; ++rows){
    for (int cols = rows; cols <= size + 1 - rows; ++cols){
      if (cols * rows >= size) {
        if (rows >= 4 && cols >= 5)
          break;
        int empty = (rows * cols) - size;
        string board = "r" + to_string(rows) + "c" + to_string(cols);
        for (int i = 0; i < checkers.size(); ++i)
          this->BuildBoard(cols, 0, empty, board, boards, 0, checkers[i]);
      }
    }
  }
}

/// Hash Function ///
// djb2 hashing as described in: http://www.cse.yorku.ca/~oz/hash.html
ulli Database::djb2(string board) {
  ulli hash = 5381;
  for (int i=0; i < board.size(); ++i)
    hash = ((hash << 5) + hash) + board[i];
  return hash;
}

/// Utility functions for inverse and symmetry
string Database::Inverse(string board){
  int cols = board[3] - '0';
  int rows = board[1] - '0';
  string inverse_string = "r" + to_string(rows) + "c" + to_string(cols);
  for (int i = 4; i < board.size(); i++){
    if (board[i] == 'o')
      inverse_string += 'x';
    else if (board[i] == 'x')
      inverse_string += 'o';
    else if (board[i] == '.')
      inverse_string += '.';
  }
  return inverse_string;
}

string Database::Symmetry(string board) {
  int cols = board[3] - '0';
  int rows = board[1] - '0';
  string symmetric_string = "r" + to_string(cols) + "c" + to_string(rows);
  for (int i = 0; i < cols; i++){
    for (int j = 0; j < rows; j++){
      symmetric_string += board[4 + j*cols + i];
    }
  }
  return symmetric_string;
}

/// Database Summary
void Database::AddToSummary(ASvalue exact_value, int print, string board) {
  static int up[9] = {0};
  static int star[3] = {0};
  static int zero = 0;
  static int switches = 0;
  vector<string> upc = {"↑3", "↑2", "↑", "↑+↑3", "↑+↑2", "↑↑", "↑↑+↑3", "↑↑+↑2", "↑↑↑"};
  vector<string> starc = {"*", "↑*", "↑↑*"};
  if (!print) {
    if (exact_value == rf.zero.value)
      zero += 1;
    else if (exact_value == rf.star.value)
      star[0] += 1;
    else if (exact_value == rf.ups.value || exact_value == rf.dps.value) {
      star[1] += 1;
      if (star[1] % 2000 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << starc[1] << endl;
      }
    }
    else if (exact_value == rf.upups.value || exact_value == rf.dpdps.value) {
      star[2] += 1;
      if (star[2] % 2000 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << starc[2] << endl;
      }
    }
    else if (exact_value == rf.u3.value || exact_value == rf.d3.value) {
      up[0] += 1;
      if (up[0] % 100 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[0] << endl;
      }
    }
    else if (exact_value == rf.u2.value || exact_value == rf.d2.value) {
      up[1] += 1;
      if (up[1] % 500 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[1] << endl;
      }
    }
    else if (exact_value == rf.u.value || exact_value == rf.d.value)
      up[2] += 1;
    else if (exact_value == rf.upu3.value || exact_value == rf.dpd3.value)
      up[3] += 1;
    else if (exact_value == rf.upu2.value || exact_value == rf.dpd2.value) {
      up[4] += 1;
      if (up[4] % 750 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[4] << endl;
      }
    }
    else if (exact_value == rf.upu.value || exact_value == rf.dpd.value) {
      up[5] += 1;
      if (up[5] % 500 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[5] << endl;
      }
    }
    else if (exact_value == rf.upupu3.value || exact_value == rf.dpdpd3.value)
      up[6] += 1;
    else if (exact_value == rf.upupu2.value || exact_value == rf.dpdpd2.value) {
      up[7] += 1;
      if (up[7] % 100 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[7] << endl;
      }
    }
    else if (exact_value == rf.upupu.value || exact_value == rf.dpdpd.value) {
      up[8] += 1;
      if (up[8] % 600 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << upc[8] << endl;
      }
    }
    else if (exact_value == rf.unkn.value) {
      switches += 1;
      if (switches % 300 == 0) {
        Clobber(board).PrintBoard();
        cout << board << endl;
        cout << "Switch" << endl;
      }
    }
  }
  else {
    cout << "Number of 0's: " << zero << endl;
    for (int i = 0; i < 3; ++i)
      cout << "Number of " << starc[i] << ": " << star[i] << endl;
    for (int i = 0; i < 9; ++i)
      cout << "Number of " << upc[i] << ": " << up[i] << endl;
    cout << "Number of Switches: " << switches << endl;
  }
}
