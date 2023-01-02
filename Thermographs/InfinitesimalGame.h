#ifndef INFINITESIMALGAME_H
#define INFINITESIMALGAME_H

#include "ASvalue.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
class InfinitesimalGame {
public:
    vector<InfinitesimalGame*> LeftOptions;
    vector<InfinitesimalGame*> RightOptions;
    string name;
    int LeftStop=1;
    int RightStop=-1;
    ASvalue value;

    InfinitesimalGame(ASvalue val, string name);
    InfinitesimalGame(InfinitesimalGame* LeftOption, InfinitesimalGame* RightOption,
                      int ls, int rs, ASvalue val, string name);
    InfinitesimalGame(InfinitesimalGame* LeftOption, vector<InfinitesimalGame*> RightOptions,
                      int ls, int rs, ASvalue val, string name);
    InfinitesimalGame(vector<InfinitesimalGame*> LeftOptions, InfinitesimalGame* RightOption,
                      int ls, int rs, ASvalue val, string name);
    InfinitesimalGame(vector<InfinitesimalGame*> LeftOptions, vector<InfinitesimalGame*> RightOptions,
                      int ls, int rs, ASvalue val, string name);
    void PrintGame();
};


#endif //INFINITESIMALGAME_H
