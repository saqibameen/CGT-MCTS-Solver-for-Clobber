#ifndef DATABASEENTRY_H
#define DATABASEENTRY_H

#include "ASvalue.h"
#include "../Clobber.h"
#include <vector>

using namespace std;
class DatabaseEntry{
private:
    DatabaseEntry* Next;
public:
   
    unsigned long long int key;
    int flag; // flag to indicate exact value or not. If 1, the exact value is available
    ASvalue exact_value;
    ASvalue lower_bound;
    ASvalue upper_bound;
    string board;

    DatabaseEntry(){};
    DatabaseEntry(unsigned long long int k, int flag, ASvalue exact, ASvalue lower, ASvalue upper, string brd){
      this->key = k;
      this->flag =flag;
      this->exact_value = exact;
      this->lower_bound = lower;
      this->upper_bound = upper;
      this->board = brd;
      this->Next = NULL;
    }

    ~DatabaseEntry(){};

    DatabaseEntry* GetNext(){
      return this->Next;
    }

    void SetNext(DatabaseEntry* next){
      DatabaseEntry::Next = next;
    }
};
#endif //DATABASEENTRY_H