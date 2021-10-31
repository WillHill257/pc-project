#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <string>
#include <vector>

using namespace std;

/*

ARCHITECTURE:
- divide the rows amongst the processes, in order of rank. The last process will have any extras
- have a logical "ring" interconnect network
- each process will receive the prev proc last row, and send the last row of curr proc

*/

int totalRows;
int totalColumns;
int localRows;

// convert a 2d coordinate to a 1d value
int convertToIndex(const int row, const int column) {
  return row * totalColumns + column;
}

// print the 2d board
void printBoard(const vector<int> &board) {
  for (int i = 0; i < board.size(); i++) {
    printf("%d", board[i]);
    if ((i + 1) % totalColumns == 0) {
      printf("\n");
    }
  }
}

// apply the Game-of-Life rules
bool cellNextValue(vector<int> &board, vector<int> &prevRow, vector<int> &nextRow, const int row, const int col) {
  // track the number of live neighbours
  int numAlive = 0;

  // we have 8 neighbours
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) {
        // this is the current cell
        continue;
      }

      // get the neighbour's row and col
      // mod to achieve wraparound
      int r = (i + row);  // don't mod because the wraparound is to other processes
      int c = (j + col + totalColumns) % totalColumns;

      // keep track of the number of live neighbours
      if (r < 0) {
        numAlive += prevRow[c];
      } else if (r >= localRows) {
        numAlive += nextRow[c];
      } else {
        numAlive += board[convertToIndex(r, c)];
      }
    }
  }

  if (board[convertToIndex(row, col)]) {
    // current cell is ALIVE

    if (numAlive == 2 || numAlive == 3) {
      // A live cell remains alive if there are 2 or 3 live neighbours;
      return true;
    } else {
      // A live cell dies if the number of live neighbours is less than 2 (loneliness) or greater than 3 (over-crowding)
      return false;
    }
  } else {
    // current cell is DEAD
    if (numAlive == 3) {
      // A dead cell will birth a new cell if there are exactly 3 live neighbours.
      return true;
    } else {
      return false;
    }
  }
}

void communicateWithPrevious(vector<int> &localBoard, vector<int> &prevRow, const int prev, const int generation) {
  // send the first row to the previous process
  // get the last row from the previous process
  MPI_Sendrecv(localBoard.data(), totalColumns, MPI_INT, prev, generation,  // send
               prevRow.data(), totalColumns, MPI_INT, prev, generation,     // receive
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void communicateWithNext(vector<int> &localBoard, vector<int> &nextRow, const int next, const int generation) {
  // send the last row to the next process
  // get the first row from the next process
  MPI_Sendrecv(localBoard.data() + localBoard.size() - totalColumns, totalColumns, MPI_INT, next, generation,  // send
               nextRow.data(), totalColumns, MPI_INT, next, generation,                                        // receive
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void playGame(const int rank, const int numProcs, const int generations, vector<int> &localBoard) {
  // determine the communication partners
  int prev = (rank - 1 + numProcs) % numProcs;
  int next = (rank + 1 + numProcs) % numProcs;

  // prepare the structures to play the game
  vector<int> prevRow, nextRow, nextGeneration;
  prevRow.resize(totalColumns);
  nextRow.resize(totalColumns);
  nextGeneration.resize(localRows * totalColumns);

  // play the game
  for (int i = 0; i < generations; i++) {
    // avoid deadlock
    if (rank % 2 == 0) {
      communicateWithPrevious(localBoard, prevRow, prev, i);
      communicateWithNext(localBoard, nextRow, next, i);
    } else {
      communicateWithNext(localBoard, nextRow, next, i);
      communicateWithPrevious(localBoard, prevRow, prev, i);
    }

    // for each cell on the local board
    for (int row = 0; row < localRows; row++) {
      for (int col = 0; col < totalColumns; col++) {
        // can now perform the update
        nextGeneration[convertToIndex(row, col)] = cellNextValue(localBoard, prevRow, nextRow, row, col) ? 1 : 0;
      }
    }

    // have determined the next generation of the board - make it active
    localBoard.swap(nextGeneration);
  }
}

int main(int argc, char *argv[]) {
  // initialise mpi environment
  MPI_Init(&argc, &argv);

  int rank, numProcs;

  // get the number of processes and this process's rank
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // check we have the arguments we need
  if (argc < 5) {
    if (rank == 0) printf("Usage: %s <rows> <columns> <seed> <generation>\n", argv[0]);
    MPI_Finalize();
    return 0;
  }

  // get the arguments
  totalRows = atoi(argv[1]);
  totalColumns = atoi(argv[2]);
  int seed = atoi(argv[3]);
  int generations = atoi(argv[4]);

  // first process will generate the game board and then distribute it
  vector<int> board, localBoard;
  if (rank == 0) {
    board.resize(totalRows * totalColumns);

    // initialise the board using the seed
    srand(seed);
    for (int i = 0; i < board.size(); i++) {
      int value = ((double)rand() / RAND_MAX) >= 0.5 ? 1 : 0;
      board[i] = value;
    }

    // print the initial board
    printBoard(board);
  }

  // determine the number of rows per process
  localRows = round((double)totalRows / numProcs);
  int lastRows = totalRows - localRows * (numProcs - 1);

  // distribute the rows
  // determine the number of elements to send to each process, and the offset in the vector
  int sendcounts[numProcs];
  int displs[numProcs];
  sendcounts[0] = localRows * totalColumns;
  displs[0] = 0;
  for (int i = 1; i < numProcs; i++) {
    sendcounts[i] = localRows * totalColumns;
    displs[i] = displs[i - 1] + totalColumns * localRows;
  }
  sendcounts[numProcs - 1] = lastRows * totalColumns;

  // the last process picks up any straggler rows
  if (rank == numProcs - 1) {
    localRows = lastRows;
  }

  // resize the local boards to hold the appropriate number of cells
  localBoard.resize(localRows * totalColumns);

  // distribute the rows
  MPI_Scatterv(board.data(), sendcounts, displs, MPI_INT, localBoard.data(), localRows * totalColumns, MPI_INT, 0, MPI_COMM_WORLD);

  // play the game
  playGame(rank, numProcs, generations, localBoard);

  // gather the localBoards
  MPI_Gatherv(localBoard.data(), localRows * totalColumns, MPI_INT, board.data(), sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("\n");
    printBoard(board);
  }

  // gracefully exit the mpi environment
  MPI_Finalize();
  return 0;
}

// if (rank == 0) {
//   string counts, displacements;
//   for (int i = 0; i < numProcs; i++) {
//     counts += to_string(sendcounts[i]) + ", ";
//     displacements += to_string(displs[i]) + ", ";
//   }
//   printf("Sendcounts = {%s}\nDisplacements = {%s}\n", counts.c_str(), displacements.c_str());
// }

// string out = "Process " + to_string(rank) + ":\n";
// for (int i = 0; i < localBoard.size(); i++) {
//   out += to_string(localBoard[i]);
//   if ((i + 1) % totalColumns == 0) out += "\n";
// }
// printf("%s\n", out.c_str());