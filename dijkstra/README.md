# Dijkstra's Single Source Shortest Path (SSSP) Algorithm

## William Hill (2115261)

### Folder Structure

- Makefile: `Makefile`
- Random Graph Generator: `graphGenerator.cpp`
- Serial (Baseline) Implementation: `serial.cpp`
- Parallel (MPI) Implementation: `mpi.cpp`
- Parallel (OpenMP) Implementation: `omp.cpp`
- Run Script: `run.sh`
- Slurm Job Script: `dijkstra.slurm`
- Input Graph Folder (of an adjacency matrix representation): `graphs/`
  - graph files (format: `numberOfNodes-edgeDensity.txt`) (e.g. `200-90.txt`, `1000-35.txt`)
- Serial Output Folder (of shortest path vectors): `serial-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-200-90.txt`)
- Parallel (MPI) Output Folder (sortest path vectors): `mpi-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-200-90.txt`)
- Parallel (OpenMP) Output Folder (sortest path vectos): `omp-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-200-90.txt`)
- Job (slurm) output folder (contains output files from the cluster): `output/`
- Job (slurm) error folder (contains error files from the cluster): `error/`
- Script to extract plottable data from output files (this changes on the fly, and shouldn't be run): `extractResults.cpp`
- Folder of plottable data files: `data/`
- Folder of graphs (produced from `data/`): `images/`

_Note: the output files will be generated when you run the job script_

### To generate graphs

In the root of the Dijkstra folder:

1. `make graphGenerator`
2. `./graphGenerator <number of vertices> <probability of edge appearing> <output filename>`
3. Example usage: `./graphGenerator 640 0.35 640-35.txt`

**Note: the graph will be stored in the `graphs/` folder**

### To run Dijkstra's Algorithm

We will specify the filename of the graph to solve **(which must have already been created)**, the source node, the number of PEs we will be using (only meaningful if we run in parallel), as well as if we want to run the serial and/or parallel verions:

1. `chmod 755 run.sh`
2. `./run.sh <filename> <start node> <num threads/processes> <run serial [y/n]> <run parallel [y/n]>`
3. Example usage: `./run.sh 640-35.txt 157 8 y y`

**Note: the serial version must have been run in order to verify parallel correctness - you can't only run the parallel verion without ever running the serial version**

**Note: the run script will verify the parallel correctness, but the serial version needs to be manually compared to an online calculator to verify serial correctness:**

- <a href="https://graphonline.ru/en/">Dijkstra's Algorithm Solver</a>
