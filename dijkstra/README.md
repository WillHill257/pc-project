# Dijkstra'a Single Source Shortest Path (SSSP) Algorithm

## William Hill (2115261)

### Folder Structure:

- Makefile: `Makefile`
- Random Graph Generator: `graphGenerator.cpp`
- Serial (Baseline) Implementation: `serial.cpp`
- Parallel (MPI) Implementation: `mpi.cpp`
- Parallel (OpenMP) Implementation: `omp.cpp`
- Run Script: `run.sh`
- Slurm Job Script: `dijkstra.slurm`
- Input Graph Folder (of an adjacency matrix representation): `graphs/`
  - graph files (e.g. `one.txt`, `two.txt`)
- Serial Output Folder (of shortest path vectors): `serial-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-one.txt`)
- Parallel (MPI) Output Folder (sortest path vectors): `mpi-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-one.txt`)
- Parallel (OpenMP) Output Folder (sortest path vectos): `omp-output/`
  - filenames will be the input graph filename, with the starting node as the prefix (e.g. `20-one.txt`)

_Note: the output files will be generated when you run the job script_

### To run:
