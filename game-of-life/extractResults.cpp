#include <fstream>
#include <iostream>

using namespace std;

int main() {
  ifstream Data("./output/mscluster39.149384.out");
  ofstream Out("./data/efficiency.txt");  // todo

  string line;
  for (int j = 0; j < 11; j++) getline(Data, line);
  for (int j = 0; j < 6; j++) {
    double serialTime;
    getline(Data, line);
    getline(Data, line);
    getline(Data, line);
    serialTime = stod(line.substr(25, line.size() - 2 - 25));
    // cout << serialTime << endl;
    getline(Data, line);
    getline(Data, line);

    for (int i = 2; i <= 20; i += 2) {
      getline(Data, line);
      getline(Data, line);
      double parallelTime = stod(line.substr(27, line.size() - 2 - 27));
      double speedup = serialTime / parallelTime;
      Out << speedup / i;
      if (i != 20) Out << ",";
      // cout << parallelTime << endl;
      getline(Data, line);
      getline(Data, line);
      getline(Data, line);
    }
    getline(Data, line);
    Out << endl;
  }

  Data.close();
  Out.close();
  return 0;
}
