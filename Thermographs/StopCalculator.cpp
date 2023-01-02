
#include "StopCalculator.h"

// For a temperature of -1, the formulas are recursively defined as:
// RS(G) = max LS(G^R) - 1, where G^R is a right option of G
// LS(G) = min LS(G^L) + 1
// RS(0) = LS(0) = 0


int StopCalculator::GetLeftStop(Clobber *c, InfinitesimalGame *infg) {
  vector<int> moves;
  c->GetMoves(moves, 0);
  if( infg->LeftOptions.empty() && moves.empty()) // If there are no more moves in c or infg, the stop is 0
    return 0;
  else if (moves.empty()) // If no more moves in c, but still moves in infg, we can acces the stop immediately
    return infg->LeftStop;

  int leftstop = 0; // The left stop can't go below 0 in an all-small game
  for (int i = 0; i < 2; ++i){
    if (i==1){ // we enter this second
      for (int j = 0; j < infg->LeftOptions.size(); ++j){
        leftstop = this->GetRightStop(c, infg->LeftOptions[j]) + 1;
        if (leftstop == 1)
          return leftstop; // We know that the maximum leftstop is 1, so we can stop once we find it
      }
    } else if (i==0){
      for (int k = 0; k < moves.size(); ++k){
        c->Move(moves[k]);
        leftstop = this->GetRightStop(c, infg) + 1;
        c->ReverseMove(moves[k]);
        if (leftstop == 1)
          return leftstop;
      }
    }
  }
  return leftstop;
}


int StopCalculator::GetRightStop(Clobber *c, InfinitesimalGame *infg) {
  vector<int> moves;
  c->GetMoves(moves, 1);
  if( infg->RightOptions.empty() && moves.empty()) // If there are no more moves in c or infg, the stop is 0
    return 0;
  else if (moves.empty()) // If no more moves in c, but still moves in infg, we can acces the stop immediately
    return infg->RightStop;

  int rightstop = 0; // The right stop can't go above 0 in an all-small game
  for (int i = 0; i < 2; ++i){
    if (i==1){ // we enter this second
      for (int j = 0; j < infg->RightOptions.size(); ++j){
        rightstop = this->GetLeftStop(c, infg->RightOptions[j]) - 1;
        if (rightstop == -1) // We know that the minimum rightstop is -1, so we can stop once we find it
          return rightstop;
      }
    } else if (i==0){
      for (int k = 0; k < moves.size(); ++k){
        c->Move(moves[k]);
        rightstop = this->GetLeftStop(c, infg) - 1;
        c->ReverseMove(moves[k]);
        if (rightstop == -1)
          return rightstop;
      }
    }
  }
  return rightstop;
}
