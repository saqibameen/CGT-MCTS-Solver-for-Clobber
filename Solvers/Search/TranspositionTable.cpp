#include "TranspositionTable.h"

using namespace std;

// Initializes the table with default size
TranspositionTable::TranspositionTable() {
  this->TransTable = new HashEntry[DEFAULT_HASHSIZE];
          //(HashEntry*) malloc(sizeof(HashEntry)*DEFAULT_HASHSIZE);
  this->size = DEFAULT_HASHSIZE;
}

// Initializes the table with provided size
TranspositionTable::TranspositionTable(int sz) {
  this->TransTable = new HashEntry[sz];
          // (HashEntry*) malloc(sizeof(HashEntry)*sz);
  this->size = sz;
}

TranspositionTable::~TranspositionTable() {
  delete [] TransTable;
}

// Print table statistics
void TranspositionTable::PrintTableStats() {
  cout << "Size: " << this->size << endl;
  cout << "Stored: " << this->stored << endl;
  cout << "Collisions: " << this->collisions << endl;
  cout << "Replaced: " << this->replaced << endl;
}

// Returns whether or not a value is in the table
bool TranspositionTable::LookUpKey(ulli key, int player) {
  return (&this->TransTable[key % this->size])->zobrist[player] == key;
}

// Compares the stored key with an input key and keeps the one with the highest depth
void TranspositionTable::KeepBestKey(ulli  key, int depth, int value, int bestMove, int player) {
  HashEntry *h = &this->TransTable[key % this->size];
  int collision = (((h)->zobrist[player] != key) && ((h)->zobrist[player]) != 0);
  if (collision) {
    this->collisions += 1;
    if (depth > (*h).depth[player]){
      this->replaced += 1;
      this->RecordHashEntry(key, depth, value, bestMove, player);
    }
  } else {
    this->RecordHashEntry(key, depth, value, bestMove, player);
  }
}

// Returns the value of the game corresponding to the key
int TranspositionTable::RetrieveValue(ulli key, int player) {
  return (this->TransTable[key % this->size]).val[player];
}

// Records a hash entry in the hash table
void TranspositionTable::RecordHashEntry(ulli key, int depth, int value, int bestMove, int player) {
  this->stored += 1;
  HashEntry *h = &this->TransTable[key % this->size];
  (*h).zobrist[player] = key;
  (*h).depth[player] = depth;
  (*h).val[player] = value;
  (*h).bestmove[player] = bestMove;
}

// Prints the Hash Entry
void TranspositionTable::PrintHashEntry(ulli key) {
  HashEntry *h = &this->TransTable[key % this->size];
  cout << "Entry: Black player, White player" << endl;
  cout << "Key: " << (*h).zobrist[0] << ", " << (*h).zobrist[1] << endl;
  cout << "depth: " << (*h).depth[0] << ", " << (*h).depth[1] << endl;
  cout << "value: " << (*h).val[0] << ", " << (*h).val[1] << endl;
  cout << "best move: " << (*h).bestmove[0] << ", " << (*h).bestmove[1] << endl;
}
