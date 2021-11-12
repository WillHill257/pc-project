# Conway's Game of Life

## William Hill (2115261)

### Folder Structure:

- Makefile: `Makefile`
- Serial (Baseline) Implementation: `serial.cpp`
- Parallel (MPI) Implementation: `parallel.cpp`
- Run Script: `run.sh`
- Serial Output File (initial and final boards): `serial-output.txt`
- Parallel Output File (initial and final boards): `parallel-output.txt`
- Slurm Job Script: `game-of-life.slurm`
- Job (slurm) output folder (contains output files from the cluster): `output/`
- Job (slurm) error folder (contains error files from the cluster): `error/`
- Script to extract plottable data from output files (this changes on the fly, and shouldn't be run): `extractResults.cpp`
- Folder of plottable data files: `data/`
- Folder of graphs (produced from `data/`): `images/`

_Note: the serial and parallel output files will be generated when you run the job script_

### To run:

We will specify the size of the game board, and provide a random seed that will be used to generate the same board for the serial and parallel versions. We will also need to specify how many generations to run the game for, the number of processes to use (only meaningful if we run in parallel), and whether we want to run the serial and/or parallel versions:

1. `chmod 755 run.sh`
2. `./run.sh <rows> <columns> <seed> <generations> <number of processes> <run serial? [y/n]> <run parallel? [y/n]>`
3. Example usage: `./run.sh 100 200 234 10 8 y y`

**Note: the serial version must have been run in order to verify parallel correctness - you can't only run the parallel verion without ever running the serial version**

**Note: the run script will verify the parallel correctness, but the serial version needs to be run with it's visualiser to verify serial correctness:**

1. `make serial`
2. `./serial <rows> <columns> <seed> <generations> v`
3. Example usage: `./serial 20 40 234 100 v`
