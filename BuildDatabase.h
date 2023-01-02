#ifndef BUILDDATABASE_H
#define BUILDDATABASE_H

#include "Thermographs/ASvalue.h"
#include "Thermographs/Database.h"
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;

void build_database() {
  Database db = Database();
  high_resolution_clock::time_point db_t1 = high_resolution_clock::now();
  int save = 1;
  db.ComputeDatabase(2, save);
  db.ComputeDatabase(3, save);
  db.ComputeDatabase(4, save);
  db.ComputeDatabase(5, save);
  db.ComputeDatabase(6, save);
  db.ComputeDatabase(7, save);
  db.ComputeDatabase(8, save);
  high_resolution_clock::time_point db_t2 = high_resolution_clock::now();
  cout << "Running time of the database in milliseconds: ";
  cout <<  duration_cast<milliseconds>(db_t2 - db_t1).count() << endl;
  cout << "Stored entries: " << db.stored << endl;
  cout << "Number of collisions: " << db.collisions <<  "\n\n";
}
#endif //BUILDDATABASE_H
