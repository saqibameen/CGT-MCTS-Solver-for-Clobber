#ifndef STOPCALCULATOR_H
#define STOPCALCULATOR_H

#include "../Clobber.h"
#include "InfinitesimalGame.h"
#include "ASvalue.h"
#include "InfGamesDefinitions.h"
#include <vector>

class StopCalculator {
public:
    int GetLeftStop(Clobber* c, InfinitesimalGame* infg);
    int GetRightStop(Clobber* c, InfinitesimalGame* infg);
};

#endif //STOPCALCULATOR_H
