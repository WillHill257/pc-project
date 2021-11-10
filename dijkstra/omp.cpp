#include <omp.h>
#include <stdlib.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

/*

General Idea:
  - cannot parallel different iterations; can only parallelise within an iteration
  - steps we can parallelise
    1) finding next min node (to close)
    2) updating all the path lengths

*/

const int averageIterations = 5;
const std::string inputPath = "graphs/";
const std::string outputPath = "omp-output/";

// check that two arrays are equal
bool compareArrays(const std::vector<int> &first, const std::vector<int> &second) {
  // if they are different sizes, can't possibly be equal
  if (first.size() != second.size()) return false;

  // check the elements
  for (int i = 0; i < first.size(); i++) {
    if (first[i] != second[i]) return false;
  }

  // all the elements are the same
  return true;
}

// find the shortest paths from the start node to all other nodes
void dijstra(const std::vector<std::vector<int>> &adjacencyMatrix, std::vector<int> &distanceArray) {
  // a set of nodes that we know the shortest path to
  std::unordered_set<int> terminalNodes;

  // loop while we have not found all the shortest paths
  while (terminalNodes.size() != distanceArray.size()) {
    int node;
    int overallMinDistance = INT32_MAX;
#pragma omp parallel shared(terminalNodes, adjacencyMatrix, distanceArray, node, overallMinDistance) default(none)
    {
      // find the node with the shortest path that we have not visited yet
      // these values are private to each thread
      int minNode = -1;
      int minDistance = INT32_MAX;

// each thread finds a minimum in its portion
#pragma omp for
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

#pragma omp critical
      {
        // each thread checks if its minNode is the overall min
        if (minDistance < overallMinDistance) {
          overallMinDistance = minDistance;
          node = minNode;
        }
      }

// wait for all the nodes to catch up (now have the next node to close)
#pragma omp barrier

// visit this node
#pragma omp single
      terminalNodes.insert(node);

// loop through all its neighbours
#pragma omp for schedule(static)  // static scheduling means false sharing becomes negligible on large graphs
      for (int i = 0; i < distanceArray.size(); i++) {
        if (adjacencyMatrix[node][i] != 0) {
          // an edge exists between the two nodes
          if (terminalNodes.find(i) == terminalNodes.end()) {
            // we have not closed the neighbour yet

            // update the shortest path to the neighbour, if it is shorter
            distanceArray[i] = std::min(distanceArray[i], distanceArray[node] + adjacencyMatrix[node][i]);
          }
        }
      }
    }  // parallel
  }    // while
}  // function

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <graph filename> <start node>" << std::endl;
    return 0;
  }

  // get the command line arguments
  std::string filename(argv[1]);
  const int startNode = atoi(argv[2]);

  // read in the graph from the file
  std::ifstream GraphIn(inputPath + filename);

  int totalNodes;
  GraphIn.ignore(INT32_MAX, '\n');  // ignore the first line
  GraphIn >> totalNodes;

  // if the start vertex is >= than the number of vertices, throw error
  if (startNode < 0 || startNode >= totalNodes) {
    std::cout << "Please choose a valid start vertex (i.e. a value between 0 and " << totalNodes - 1 << ", inclusive)" << std::endl;
    GraphIn.close();
    return 0;
  }

  // read in the adjacency matrix
  std::vector<std::vector<int>> adjacencyMatrix(totalNodes, std::vector<int>(totalNodes, 0));  // initialise all elements to 0
  for (int row = 0; row < totalNodes; row++) {
    for (int col = 0; col < totalNodes; col++) {
      GraphIn >> adjacencyMatrix[row][col];
    }
  }

  // close the input file
  GraphIn.close();

  // keep track of the total running time
  u_int64_t runTime = 0;

  // keep track of the first distance array returned - use to compare against other iterations
  std::vector<int> overallDistance;

  // get an average runtime
  for (int iter = 0; iter < averageIterations; iter++) {
    // initialise the distance array
    std::vector<int> distanceArray(totalNodes, INT32_MAX);
    distanceArray[startNode] = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    // run dijsktra
    dijstra(adjacencyMatrix, distanceArray);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    runTime += duration.count();

    // validate this iteration's answer
    if (iter == 0) {
      // first time running - save to initial vector
      distanceArray.swap(overallDistance);
    } else {
      if (!compareArrays(distanceArray, overallDistance)) {
        std::cout << "Multiple different runs are returning different answers." << std::endl;
        return 0;
      }
    }
  }

  std::cout << "OpenMP average running time: " << (double)runTime / averageIterations << "ms" << std::endl;

  // print result to file
  std::ofstream GraphOut(outputPath + std::to_string(startNode) + "-" + filename);

  for (int value : overallDistance) {
    GraphOut << value << "\n";
  }

  // close the output file
  GraphOut.close();

  return 0;
}
