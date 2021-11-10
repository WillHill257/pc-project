
# make sure we have the correct arguments
if [ "$#" != 5 ]
then
  echo "The input graph must already have been generated!"
  echo ""
  echo "Specify the input graph filename (no path), the vertex to start from, and the number of threads/processes to run"
  echo ""
  echo "Usage: ${0} <filename> <start node> <num threads/processes> <run serial [y/n]> <run parallel [y/n]>"
  exit
fi

# we have the correct arguments
filename=$1
startNode=$2
numProcs=$3
runSerial=$4
runParallel=$5

inputPath="graphs/"
serialOutput="serial-output/${startNode}-${filename}"
mpiOutput="mpi-output/${startNode}-${filename}"
ompOutput="omp-output/${startNode}-${filename}"

# make the serial and parallel versions
echo "Making executables"
make clean
make
echo "Done making"
echo

# run the serial version, pipe the output to the relevant file
if [ $runSerial == "y" ]
then
  echo "Running serial"
  rm -f $serialOutput
  ./serial $filename $startNode
  echo "Done serial"
  echo
fi

# run the parallel version, pipe the output to the relevant file
if [ $runParallel == "y" ]
then
  echo "Running MPI"
  rm -f $mpiOutput
  mpirun -np $numProcs ./mpi $filename $startNode
  echo "Done MPI"
  echo

  echo "Running OpenMP"
  rm -f $ompOutput
  export OMP_NUM_THREADS=$numProcs
  ./omp $filename $startNode
  echo "Done OpenMP"
  echo
fi

# compare the serial and parallel outputs to VERIFY
# only compare if we want to check the parallel version
if [ $runParallel == "y" ] 
then
  # check mpi correctness
  DIFF=$(diff $serialOutput $mpiOutput)
  if [ "$DIFF" ]
  then 
    echo "The serial and MPI outputs are different!"
  else
    echo "The serial and MPI outputs are the same and correct!"
  fi

  # check omp correctness
  DIFF=$(diff $serialOutput $ompOutput)
  if [ "$DIFF" ]
  then 
    echo "The serial and OpenMP outputs are different!"
  else
    echo "The serial and OpenMP outputs are the same and correct!"
  fi
fi

# clean up
make clean