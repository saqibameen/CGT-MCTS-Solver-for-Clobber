#include "ASvalue.h"
using namespace std;

ASvalue::ASvalue() {}

ASvalue::ASvalue(int iu, int iu2, int iu3, int istar) {
  this->u = iu;
  this->u2 = iu2;
  this->u3 = iu3;
  this->star = istar;
}

// Game addition
ASvalue ASvalue::operator+(const ASvalue &b) {
  ASvalue c;
  c.u = this->u + b.u;
  c.u2 = this->u2 + b.u2;
  c.u3 = this->u3 + b.u3;
  c.star = (this->star + b.star) % 2;
  return c;
}

// Game comparison
int ASvalue::operator==(const ASvalue &b) {
  return ((this->u == b.u) && (this->u2 == b.u2) && (this->u3 == b.u3) && (this->star == b.star));
}

// Get the inverse of a game
ASvalue ASvalue::Inverse() {
  ASvalue c;
  c.u = -this->u;
  c.u2 = -this->u2;
  c.u3 = -this->u3;
  c.star = this->star;
  return c;
}

// Returns the outcome class of an ASvalue
int ASvalue::Outcome(int player) {
  int win[2] = {0, 1};
  if (this->u >= 2)
    return 0;
  else if (this->u <= -2)
    return 1;
  else if (this->star == 1)
    return player;
  else if (this->u == 1 || this->u == -1)
    return win[(this->u > 0?0:1)];
  else if (this->u2 >= 1 || this->u2 <= -1)
    return win[(this->u2 > 0?0:1)];
  else if (this->u3 >= 1 || this->u3 <= -1)
    return win[(this->u3 > 0?0:1)];
  else
    return (player-1)*-1; // The other player
}

void ASvalue::PrintValue() {
  cout << "(" << this->u << "↑) + (" << this->u2 << "↑^2) + (" << this->u3 << "↑^3) + " << this->star << "*" << endl;
}