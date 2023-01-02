#include "InfinitesimalGame.h"

InfinitesimalGame::InfinitesimalGame(ASvalue val, string name) {
  this->name = name;
  this->value = val;
};

InfinitesimalGame::InfinitesimalGame(InfinitesimalGame *LeftOption, InfinitesimalGame *RightOption, int ls, int rs,
                                     ASvalue val, string name) {
  this->LeftOptions.push_back(LeftOption);
  this->RightOptions.push_back(RightOption);
  this->LeftStop = ls;
  this->RightStop = rs;
  this->value = val;
  this->name = name;
}

InfinitesimalGame::InfinitesimalGame(InfinitesimalGame* LeftOption, vector<InfinitesimalGame*> RightOptions, int ls,
                                     int rs, ASvalue val, string name){
  this->LeftOptions.push_back(LeftOption);
  this->RightOptions.swap(RightOptions);
  this->LeftStop = ls;
  this->RightStop = rs;
  this->value = val;
  this->name = name;
}

InfinitesimalGame::InfinitesimalGame(vector<InfinitesimalGame *> LeftOptions, InfinitesimalGame *RightOption, int ls,
                                     int rs, ASvalue val, string name) {
  this->LeftOptions.swap(LeftOptions);
  this->RightOptions.push_back(RightOption);
  this->LeftStop = ls;
  this->RightStop = rs;
  this->value = val;
  this->name = name;
}

InfinitesimalGame::InfinitesimalGame(vector<InfinitesimalGame *> LeftOptions, vector<InfinitesimalGame *> RightOptions,
                                     int ls, int rs, ASvalue val, string name) {
  this->LeftOptions.swap(LeftOptions);
  this->RightOptions.swap(RightOptions);
  this->LeftStop = ls;
  this->RightStop = rs;
  this->value = val;
  this->name = name;
}

void InfinitesimalGame::PrintGame() {
  cout << "The value of " << this->name << " is: ";
  this->value.PrintValue();
  cout << "Left option(s) of " << this->name << endl;
  for (int i = 0; i < this->LeftOptions.size(); ++i)
    cout << "\t" <<  (*this->LeftOptions[i]).name << endl;

  cout << "Right option(s) of " << this->name << endl;
  for (int i =0; i < this->RightOptions.size(); ++i)
    cout << "\t" << (*this->RightOptions[i]).name << endl;
  cout << "\n";
}