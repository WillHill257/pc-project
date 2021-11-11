#include <fstream>
#include <iostream>

using namespace std;

const int minVal = 4;
const int maxVal = 5;

int main() {
  ifstream Data("./output/mscluster16.150070.out");
  ofstream Out("./data/6144-efficiencies.txt");  // todo

  string line;

  for (int j = 0; j < 11; j++) getline(Data, line);
  for (int j = 0; j < 8; j++) {
    string omp;
    string mpi;

    double serialTime;
    getline(Data, line);
    getline(Data, line);
    serialTime = stod(line.substr(29, line.size() - 2 - 29));
    // cout << serialTime << endl;
    getline(Data, line);

    for (int i = 2; i <= 20; i += 2) {
      getline(Data, line);
      getline(Data, line);
      double mpiTime = stod(line.substr(26, line.size() - 2 - 26));
      // cout << mpiTime << endl;
      double mpiSpeedup = serialTime / mpiTime;
      getline(Data, line);
      double ompTime = stod(line.substr(29, line.size() - 2 - 29));
      // cout << ompTime << endl;
      double ompSpeedup = serialTime / ompTime;

      if (minVal <= j && j <= maxVal) {
        omp += to_string(ompSpeedup / i);
        mpi += to_string(mpiSpeedup / i);
        if (i != 20) {
          omp += ",";
          mpi += ",";
        }
      }
      // cout << parallelTime << endl;
      getline(Data, line);
      getline(Data, line);
      getline(Data, line);
    }
    getline(Data, line);
    if (minVal <= j && j <= maxVal) {
      Out << mpi << endl;
      Out << omp << endl;
    }
  }

  Data.close();
  Out.close();
  return 0;
}
