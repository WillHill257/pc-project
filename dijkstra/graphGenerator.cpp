// https://www.bsmath.hu/~p_erdos/1963-04.pdf
// Erdos-Renyi Model for generating random graphs

#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// generate a random undirected and connected graph
// return an adjacency matrix
void generateRandomGraph(const int numVertices, const double probability, vector<vector<int>> &adjacencyMatrix) {
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <number of vertices> <probability of edge appearing> <output filepath>" << endl;
    return 0;
  }

  const int numVertices = atoi(argv[1]);
  const double probability = atof(argv[2]);
  string filepath(argv[3]);

  // create the adjacency matrix
  vector<vector<int>> adjacencyMatrix(numVertices, vector<int>(numVertices));

  // generate the random graph
  generateRandomGraph(numVertices, probability, adjacencyMatrix);

  // write the graph to the text file
  ofstream Graph(filepath);

  Graph << numVertices << "\n";

  for (int i = 0; i < adjacencyMatrix.size(); i++) {
    for (int j = 0; j < adjacencyMatrix[i].size(); j++) {
      Graph << adjacencyMatrix[i][j];
      if (j != adjacencyMatrix[i].size() - 1)
        Graph << " ";
    }
    if (i != adjacencyMatrix.size() - 1)
      Graph << "\n";
  }

  return 0;
}
