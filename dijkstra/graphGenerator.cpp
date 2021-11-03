// https://www.bsmath.hu/~p_erdos/1963-04.pdf
// Erdos-Renyi Model for generating random graphs

#include <stdlib.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_set>
#include <vector>

using namespace std;

const int minNum = 1;
const int maxNum = 9;

int getRandomElement(unordered_set<int> &theSet, bool removeElement) {
  // determine the position in the set of the element to choose
  int randomPosition = rand() % theSet.size();

  // get an iterator
  auto it = theSet.begin();
  advance(it, randomPosition);  // get the desired position

  // store the number at the position
  int number = *it;

  // remove the element from the set
  if (removeElement)
    theSet.erase(it);

  // rturn the number
  return number;
}

// generate a spanning tree between the vertices - will make a graph connected
void generateSpanningTree(const int numVertices, vector<vector<int>> &adjacencyMatrix) {
  // have sets of connected and unconnected nodes
  unordered_set<int> connected, unconnected;
  connected.insert(0);  // first node is trivially connected
  for (int i = 1; i < numVertices; i++) {
    // all other nodes are unconnected
    unconnected.insert(i);
  }

  // loop while there is still a node which is unconnected
  while (!unconnected.empty()) {
    // get a random element from each set
    int connectedNode = getRandomElement(connected, false);
    int unconnectedNode = getRandomElement(unconnected, true);

    // connect these two elements
    adjacencyMatrix[connectedNode][unconnectedNode] = rand() % (maxNum - minNum + 1) + minNum;
    adjacencyMatrix[unconnectedNode][connectedNode] = adjacencyMatrix[connectedNode][unconnectedNode];

    // insert the now-connected node into the connected set
    connected.insert(unconnectedNode);
  }

  /*

  we maintain two sets: one of nodes connected to each other, and one of nodes not connected to any other node
  initially, only one node is connected (trivially)

  for each iteration of the while loop, we choose a random connected and unconnected node
    we connect these two nodes with a non-zero weight
    now, the unconnected node is connected to all other nodes in the connected set

    we remove the unconnected node from the unconnected set and add it to the connected set

  consequently, for each iteration of the file loop, one more node is added to the connected subgraph.
  since there is at most a finite number of nodes, the loop will execute a finite number of times, and we are guaranteed to terminate.
  specifically, the loop will execute (nodes - 1) times, adding (nodes - 1) nodes to the connected subgraph, which initially had one node already

  therefore, the algorithm is correct.

  */
}

// generate a random undirected and connected graph
// return an adjacency matrix
void generateRandomGraph(const int numVertices, const double probability, vector<vector<int>> &adjacencyMatrix) {
  // start by generating a random spanning tree through the nodes - this will make it connected
  // then assign extra edges
  generateSpanningTree(numVertices, adjacencyMatrix);

  // loop through each row
  // loop through each columns after the main diagonal (undirected graph, so the matrix will be symmetrical)
  // randomly assign a weight
  for (int row = 0; row < adjacencyMatrix.size(); row++) {
    for (int col = row + 1; col < adjacencyMatrix[row].size(); col++) {
      // assign a weight with the given probability
      if (adjacencyMatrix[row][col] == 0 && (double)rand() / RAND_MAX < probability) {
        // assign the weight
        adjacencyMatrix[row][col] = rand() % (maxNum - minNum + 1) + minNum;

        // make the matrix symmetrical
        adjacencyMatrix[col][row] = adjacencyMatrix[row][col];
      }
    }
  }
}

void toString(vector<vector<int>> &adjacencyMatrix) {
  // print out the vertiex numbers
  for (int i = 0; i < adjacencyMatrix.size(); i++) {
    cout << i << endl;
  }

  // print out the edges
  for (int row = 0; row < adjacencyMatrix.size(); row++) {
    for (int col = row + 1; col < adjacencyMatrix[row].size(); col++) {
      // assign a weight with the given probability
      if (adjacencyMatrix[row][col] != 0) {
        cout << row << " " << col << endl;
      }
    }
  }
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
  srand(time(0));
  generateRandomGraph(numVertices, probability, adjacencyMatrix);
  // toString(adjacencyMatrix);

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
