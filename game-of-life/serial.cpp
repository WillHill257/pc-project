#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

// move cursor so that print over current board
#define cursup "\033[A"
#define curshome "\033[0;0H"

// define block to print
#define BLOCK "\u2593"

// define colours so easier to see movement of live cells
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KBLU "\x1B[34m"
#define KCYN "\x1B[36m"
#define RESET "\033[0m"

using namespace std;

int totalRows;
int totalColumns;

// convert a 2d coordinate to a 1d value
int convertToIndex(const int row, const int column) {
  return row * totalColumns + column;
}

// print the 2d board
void printVisualiser(const vector<bool> &board) {
  printf(curshome);
  for (int i = 0; i < board.size(); i++) {
    if (board[i]) {
      printf(KGRN BLOCK RESET);
    } else {
      printf(KRED BLOCK RESET);
    }
    if ((i + 1) % totalColumns == 0) {
      printf("\n");
    }
  }
}

// print the 2d board
void printBoard(const vector<bool> &board) {
  for (int i = 0; i < board.size(); i++) {
    printf("%d", board[i] == true);
    if ((i + 1) % totalColumns == 0) {
      printf("\n");
    }
  }
}

// apply the Game-of-Life rules
bool cellNextValue(vector<bool> &board, const int row, const int col) {
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
      int r = (i + row + totalRows) % totalRows;
      int c = (j + col + totalColumns) % totalColumns;

      // keep track of the number of live neighbours
      if (board[convertToIndex(r, c)]) {
        numAlive++;
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

int main(int argc, char *argv[]) {
  // check we have the arguments we need
  if (argc < 5) {
    printf("Usage: %s <rows> <columns> <seed> <generation> <OPTIONAL: visualise>\n", argv[0]);
    return 0;
  }

  // get the arguments
  totalRows = atoi(argv[1]);
  totalColumns = atoi(argv[2]);
  int seed = atoi(argv[3]);
  int generation = atoi(argv[4]);

  bool visualise = argc == 6 ? true : false;

  // create our board
  vector<bool> board, nextGeneration;
  board.resize(totalRows * totalColumns);
  nextGeneration.resize(totalRows * totalColumns);

  // initialise the board using the seed
  srand(seed);
  for (int i = 0; i < board.size(); i++) {
    bool value = ((double)rand() / RAND_MAX) >= 0.5 ? true : false;
    board[i] = value;
  }

  // print the initial board
  printBoard(board);

  // run the game for a number of iterations
  for (int iter = 0; iter < generation; iter++) {
    // determine the next generation of the board
    for (int row = 0; row < totalRows; row++) {
      for (int col = 0; col < totalColumns; col++) {
        // figure out if this cell should be alive/dead
        nextGeneration[convertToIndex(row, col)] = cellNextValue(board, row, col);
      }
    }

    // have determined the next generation of the board - make it active
    board.swap(nextGeneration);

    if (visualise) {
      printVisualiser(board);
      this_thread::sleep_for(chrono::milliseconds(500));
    }
  }

  // print the final board
  printf("\n");
  printBoard(board);

  return 0;
}