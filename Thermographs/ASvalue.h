#ifndef ASVALUE_H
#define ASVALUE_H
// Stands for all-small value

#include <iostream>

class ASvalue {
public:
    int u=0;
    int u2=0;
    int u3=0;
    int star=0;

    ASvalue();
    ASvalue(int iu, int iu2, int iu3, int istar);
    ASvalue operator+(const ASvalue& b);
    int operator==(const ASvalue& b);
    ASvalue Inverse();
    int Outcome(int player);

    void PrintValue();
};

#endif //ASVALUE_H
