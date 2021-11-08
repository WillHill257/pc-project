#include <mpi.h>
#include <stdlib.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

const int averageIterations = 5;
const std::string inputPath = "graphs/";
const std::string outputPath = "mpi-output/";

int totalNodes;
int rank;
int numProcs;

typedef struct {
  int node;      // global node number
  int distance;  // min distance to this node
} node_distance;

// build an MPI type for this specific object
void buildNodeDistanceType(int *node, int *distance, MPI_Datatype *node_distance_type) {
  // define length of each block
  int array_of_blocklengths[2] = {1, 1};

  // define the type of elements in each block
  MPI_Datatype array_of_types[2] = {MPI_INT, MPI_INT};

  // define the displacements of each element
  MPI_Aint array_of_displacements[2] = {0};
  MPI_Get_address(node, &array_of_displacements[0]);
  MPI_Get_address(distance, &array_of_displacements[1]);

  // get the relative displacement of each element
  for (int i = 1; i >= 0; i--) {
    array_of_displacements[i] -= array_of_displacements[0];
  }

  // create the struct
  MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, node_distance_type);

  // commit the type
  MPI_Type_commit(node_distance_type);
}

// determine the base number of local nodes
int determineNumLocalNodes() {
  return round((double)totalNodes / numProcs);
}

// convert 2d coord into 1d index
int convertToIndex(const int row, const int col) {
  return row * totalNodes + col;
}

// get the real node number
int convertToGlobalNode(const int nodeDisplacement) {
  int localNodes = determineNumLocalNodes();
  return localNodes * rank + nodeDisplacement;
}

// determine the node displacement in this process of the global node
int convertToLocalNode(const int globalNode) {
  int localNodes = determineNumLocalNodes();

  if (rank == numProcs - 1) {
    // last process - special number of nodes
    localNodes = totalNodes - localNodes * (numProcs - 1);
  }

  return globalNode % localNodes;
}

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

// return the node with the min distance OVERALL
node_distance pickShortestUnvisitedNode(const std::unordered_set<int> &terminalNodes, const std::vector<int> &distanceArray) {
  // loop through the distance array
  // if a value is less than the minimum, check if the node is terminal
  // if yes, skip
  // if no, record it and keep searching

  int localNode = -1;
  int minDistance = INT32_MAX;

  for (int i = 0; i < distanceArray.size(); i++) {
    // looping through all the vertices
    if (terminalNodes.find(convertToGlobalNode(i)) == terminalNodes.end()) {
      // if the current node is NOT terminal

      // compare the distance to the minimum distance
      if (distanceArray[i] < minDistance) {
        minDistance = distanceArray[i];
        localNode = i;
      }
    }
  }

  // localNode is now the minimum node
  int globalNode = convertToGlobalNode(localNode);

  // define a struct of node and distance - used for reduction
  MPI_Datatype MPI_NODE_DISTANCE;

  node_distance nodeDistance;
  nodeDistance.node = globalNode;
  nodeDistance.distance = minDistance;

  buildNodeDistanceType(&nodeDistance.node, &nodeDistance.distance, &MPI_NODE_DISTANCE);

  // now gather this new type in all processes
  std::vector<node_distance> nodeDistances;
  if (rank == 0) nodeDistances.resize(numProcs);
  MPI_Gather(&nodeDistance, 1, MPI_NODE_DISTANCE, nodeDistances.data(), 1, MPI_NODE_DISTANCE, 0, MPI_COMM_WORLD);

  // find the minimum node - each process has the same data and will find the same minimum
  node_distance minNode;
  if (rank == 0) {
    minNode = nodeDistances[0];
    for (int i = 1; i < nodeDistances.size(); i++) {
      if (nodeDistances[i].distance < minNode.distance) {
        minNode = nodeDistances[i];
      }
    }
  }

  MPI_Bcast(&minNode, 1, MPI_NODE_DISTANCE, 0, MPI_COMM_WORLD);

  MPI_Type_free(&MPI_NODE_DISTANCE);

  // return the min node
  return minNode;
}

// run parallel dijsktra
void dijsktra(const int startNode, std::vector<int> &localMatrix, std::vector<int> &distanceArray) {
  // a set of nodes that we know the shortest path to
  std::unordered_set<int> terminalNodes;

  const int minNode = convertToGlobalNode(0);
  const int maxNode = convertToGlobalNode(distanceArray.size() - 1);

  // set the start node to have a distance of 0
  if (minNode <= startNode && startNode <= maxNode) {
    distanceArray[convertToLocalNode(startNode)] = 0;
  }

  // while we have not closed all the nodes
  while (terminalNodes.size() != totalNodes) {
    // find the minimum node in this process
    // need to do this until the entire ecosystem is done (because of the collective communications)
    node_distance globalNode = pickShortestUnvisitedNode(terminalNodes, distanceArray);

    // add the node to the terminal set
    terminalNodes.insert(globalNode.node);

    // loop through all its local neighbours
    for (int i = 0; i < distanceArray.size(); i++) {
      // if an edge exists between the two nodes
      if (localMatrix[convertToIndex(i, globalNode.node)] != 0) {
        // if we have not yet closed this neighbour
        if (terminalNodes.find(convertToGlobalNode(i)) == terminalNodes.end()) {
          // then we can update its length, if it is required
          distanceArray[i] = std::min(distanceArray[i], globalNode.distance + localMatrix[convertToIndex(i, globalNode.node)]);
        }
      }
    }
  }
}

void doWork(const int startNode, std::vector<int> &adjacencyMatrix, std::vector<int> &distanceArray) {
  // ------------------ distribute nodes (rows of adjacency matrix) ------------------

  // define a new datatype (of rows)
  MPI_Datatype ROW;
  MPI_Type_contiguous(totalNodes, MPI_INT, &ROW);
  MPI_Type_commit(&ROW);

  // determine the number of nodes per process - last process makes up the deficit
  int localNodes = determineNumLocalNodes();
  int lastNodes = totalNodes - localNodes * (numProcs - 1);

  // determine the number of rows to send to each process, and the offset in the matrix
  int sendcounts[numProcs];
  int displs[numProcs];
  displs[0] = 0;
  sendcounts[0] = localNodes;
  for (int i = 1; i < numProcs; i++) {
    sendcounts[i] = localNodes;
    displs[i] = displs[i - 1] + localNodes;
  }
  sendcounts[numProcs - 1] = lastNodes;

  // the last process picks up any straggler rows
  if (rank == numProcs - 1) {
    localNodes = lastNodes;
  }

  // create local matrix
  // has localNodes rows, with totalNodes columns
  std::vector<int> localMatrix(localNodes * totalNodes, 0);

  // scatter
  MPI_Scatterv(adjacencyMatrix.data(), sendcounts, displs, ROW,  // send info
               localMatrix.data(), localNodes, ROW,              // receive info
               0, MPI_COMM_WORLD);

  // ------------------ run dijsktra ------------------
  std::vector<int> localDistance(localNodes, INT32_MAX);
  dijsktra(startNode, localMatrix, localDistance);

  // ------------------ gather results into distanceArray ------------------
  if (rank == 0) {
    // resize the array
    distanceArray.resize(totalNodes);
  }

  // gather all the local distance arrays
  MPI_Gatherv(localDistance.data(), localDistance.size(), MPI_INT,  // send info
              distanceArray.data(), sendcounts, displs, MPI_INT,    // receive info
              0, MPI_COMM_WORLD);

  MPI_Type_free(&ROW);
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  // get the rank of the current process and the number of processes in total
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // get the command line arguments
  if (argc != 3) {
    if (rank == 0) std::cout << "Usage: " << argv[0] << " <graph filename> <start node>" << std::endl;
    MPI_Finalize();
    return 0;
  }

  std::string filename(argv[1]);
  int startNode = atoi(argv[2]);

  // read in the graph
  std::vector<int> adjacencyMatrix;
  bool inputError = false;
  if (rank == 0) {
    std::ifstream GraphIn(inputPath + filename);

    GraphIn.ignore(INT32_MAX, '\n');  // ignore the first line
    GraphIn >> totalNodes;

    // if the start vertex is >= than the number of vertices, throw error
    if (startNode < 0 || startNode >= totalNodes) {
      std::cout << "Please choose a valid start vertex (i.e. a value between 0 and " << totalNodes - 1 << ", inclusive)" << std::endl;
      inputError = true;
    } else {
      // create the adjacency matrix skeleton
      // initialise all elements to 0
      adjacencyMatrix.resize(totalNodes * totalNodes, 0);

      // read in the adjacency matrix
      for (int row = 0; row < totalNodes; row++) {
        for (int col = 0; col < totalNodes; col++) {
          GraphIn >> adjacencyMatrix[convertToIndex(row, col)];  // safe to convert since rank 0 has the correct numNodes
        }
      }
    }

    // close the input file
    GraphIn.close();
  }

  // check if there were any errors with the graph
  MPI_Bcast(&inputError, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
  if (inputError) {
    MPI_Finalize();
    return 0;
  }

  // broadcast the number of nodes
  MPI_Bcast(&totalNodes, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // have now read all the input into process 0

  // get an average runtime
  u_int64_t runTime = 0;
  std::vector<int> overallDistance;  // only meaningful for process 0
  for (int iter = 0; iter < averageIterations; iter++) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // ------------------ do work ------------------
    std::vector<int> distanceArray;  // only meaningful for process 0
    doWork(startNode, adjacencyMatrix, distanceArray);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    runTime += duration.count();

    // validation across runs
    bool stopIterations = false;
    if (rank == 0) {
      // check the two distance arrays are equal
      if (iter == 0) {
        // first iteration - use this as the truth array
        distanceArray.swap(overallDistance);
      } else {
        // every other iteration - check answer against overallDistance
        if (!compareArrays(distanceArray, overallDistance)) {
          std::cout << "Multiple different runs are returning different answers." << std::endl;
          stopIterations = true;
        }
      }
    }

    if (stopIterations) {
      MPI_Finalize();
      return 0;
    }
  }

  // print average runtime and results
  if (rank == 0) {
    std::cout << "MPI average running time: " << (double)runTime / averageIterations << "ms" << std::endl;

    // print result to file
    std::ofstream GraphOut(outputPath + std::to_string(startNode) + "-" + filename);

    for (int value : overallDistance) {
      GraphOut << value << "\n";
    }

    // close the output file
    GraphOut.close();
  }

  // clean up all the processes
  MPI_Finalize();
  return 0;
}
