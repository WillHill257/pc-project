#include <stdlib.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

/*

General Idea:
  - maintain a set of vertices to which we have found the shortest path
  - have an array of the shortest distance (so far) to each vertex
  - while there are still vertices not in the minSet:
    * choose the vertex with the minimum distance
    * add this vertex to minSet (i.e. we close this vertex)
    * examine all adjacent vertices (that are not in minSet - i.e. not closed) and see if the new distance (vertex + edge weight) is the new min

  - return the distance array

*/

const int averageIterations = 5;
const string inputPath = "graphs/";
const string outputPath = "serial-output/";

// check that two arrays are equal
bool compareArrays(const vector<int> &first, const vector<int> &second) {
  // if they are different sizes, can't possibly be equal
  if (first.size() != second.size()) return false;

  // check the elements
  for (int i = 0; i < first.size(); i++) {
    if (first[i] != second[i]) return false;
  }

  // all the elements are the same
  return true;
}

int pickShortestUnvisitedNode(const unordered_set<int> &terminalNodes, const vector<int> &distanceArray) {
  // loop through the distance array
  // if a value is less than the minimum, check if the node is terminal
  // if yes, skip
  // if no, record it and keep searching

  int minNode = -1;
  int minDistance = INT32_MAX;

  for (int i = 0; i < distanceArray.size(); i++) {
    // looping through all the vertices
    if (terminalNodes.find(i) == terminalNodes.end()) {
      // if the current node is NOT terminal
      // compare the distance to the minimum distance
      if (distanceArray[i] < minDistance) {
        minDistance = distanceArray[i];
        minNode = i;
      }
    }
  }

  return minNode;
}

// find the shortest paths from the start node to all other nodes
void dijkstra(const int startNode, const vector<vector<int>> &adjacencyMatrix, vector<int> &distanceArray) {
  // a set of nodes that we know the shortest path to
  unordered_set<int> terminalNodes;

  // loop while we have not found all the shortest paths
  while (terminalNodes.size() != distanceArray.size()) {
    // find the node with the shortest path that we have not visited yet
    int node = pickShortestUnvisitedNode(terminalNodes, distanceArray);

    // visit this node
    terminalNodes.insert(node);

    // loop through all its neighbours
    for (int i = 0; i < distanceArray.size(); i++) {
      if (adjacencyMatrix[node][i] != 0) {
        // an edge exists between the two nodes
        if (terminalNodes.find(i) == terminalNodes.end()) {
          // we have not closed the neighbour yet

          // update the shortest path to the neighbour, if it is shorter
          distanceArray[i] = min(distanceArray[i], distanceArray[node] + adjacencyMatrix[node][i]);
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <graph filename> <start vertex>" << endl;
    return 0;
  }

  // get the command line arguments
  string filename(argv[1]);
  const int startVertex = atoi(argv[2]);

  // read in the graph from the file
  ifstream GraphIn(inputPath + filename);

  int numVertices;
  GraphIn.ignore(INT32_MAX, '\n');  // ignore the first line
  GraphIn >> numVertices;

  // if the start vertex is >= than the number of vertices, throw error
  if (startVertex < 0 || startVertex >= numVertices) {
    cout << "Please choose a valid start vertex (i.e. a value between 0 and " << numVertices - 1 << ", inclusive)" << endl;
    GraphIn.close();
    return 0;
  }

  // read in the adjacency matrix
  vector<vector<int>> adjacencyMatrix(numVertices, vector<int>(numVertices, 0));  // initialise all elements to 0
  for (int row = 0; row < numVertices; row++) {
    for (int col = 0; col < numVertices; col++) {
      GraphIn >> adjacencyMatrix[row][col];
    }
  }

  // close the input file
  GraphIn.close();

  // keep track of the total running time
  u_int64_t runTime = 0;

  // keep track of the first distance array returned - use to compare against other iterations
  vector<int> overallDistance;

  for (int _ = 0; _ < averageIterations; _++) {
    // initialise the distance array
    vector<int> distanceArray(numVertices, INT32_MAX);
    distanceArray[startVertex] = 0;

    auto startTime = chrono::high_resolution_clock::now();

    // find all the shortest paths
    dijkstra(startVertex, adjacencyMatrix, distanceArray);

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    runTime += duration.count();

    if (_ == 0) {
      // first time running - save to initial vector
      distanceArray.swap(overallDistance);
    } else {
      if (!compareArrays(distanceArray, overallDistance)) {
        cout << "Multiple different runs are returning different answers." << endl;
        return 0;
      }
    }
  }

  cout << "Serial average running time: " << (double)runTime / averageIterations << "ms" << endl;

  // print result to file
  ofstream GraphOut(outputPath + to_string(startVertex) + "-" + filename);

  for (int value : overallDistance) {
    GraphOut << value << "\n";
  }

  // close the output file
  GraphOut.close();

  return 0;
}
