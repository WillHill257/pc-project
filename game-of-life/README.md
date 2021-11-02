# Conway's Game of Life

## William Hill (2115261)

### Folder Structure:

- Makefile: `Makefile`
- Serial (Baseline) Implementation: `serial.cpp`
- Parallel (MPI) Implementation: `parallel.cpp`
- Run Script: `run.sh`
- Slurm Job Script: `game-of-life.slurm`
- Serial Output File (initial and final boards): `serial-output.txt`
- Parallel Output File (initial and final boards): `parallel-output.txt`

_Note: the serial and parallel output files will be generated when you run the job script_

### To run:

We will specify the size of the game board, and provide a random seed that will be used to generate the same board for the serial and parallel versions. We will also need to specify how many generations to run the game for, the number of processes to use (only meaningful if we run in parallel), and whether we want to run the serial and/or parallel versions:

`chmod 755 run.sh`

`./run.sh <rows> <columns> <seed> <generations> <number of processes> <run serial? [y/n]> <run parallel? [y/n]>`

**Note: the run script will verify the parallel correctness, but the serial version needs to be run with it's visualiser to verify serial correctness:**

`make serial`</br>
`./serial <rows> <columns> <seed> <generations> v`
