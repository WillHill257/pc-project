# Conway's Game of Life

## William Hill (2115261)

### Folder Structure:

- Makefile: `Makefile`
- Serial (Baseline) Implementation: `serial.cpp`
- Parallel (MPI) Implementation: `parallel.cpp`
- Run Script: `run.sh`
- Serial Output File (after N generations): `serial-output.txt`
- Parallel Output File (after N generations): `parallel-output.txt`

### To run:

We will specify the size of the game board, and provide a random seed that will be used to generate the same board for the serial and parallel versions. We will also need to specify how many generations to run the game for (-1 for indefinite):

`chmod 755 run.sh`

`./run.sh <rows> <columns> <seed> <generations>`
