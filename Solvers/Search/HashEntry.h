#include <vector>

#ifndef HASHENTRY_H
#define HASHENTRY_H


class HashEntry
{
public:
    HashEntry(){};
    unsigned long long int zobrist[2] = {0,0};
    int depth[2] = {0,0};
    int val[2] = {-1,-1};
    int bestmove[2] = {0,0};
};

#endif //HASHENTRY_H
