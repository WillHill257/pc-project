
# make sure we have the correct arguments
if [ "$#" != 7 ]
then
  echo "We will specify the size of the game board, and provide a random seed that will be used to generate the same board for the serial and parallel versions. We will also need to specify how many generations to run the game for:"
  echo
  echo "Usage: ${0} <rows> <columns> <seed> <generations> <number of processes> <run serial? [y/n]> <run parallel? [y/n]>"
  exit
fi

# we have the correct arguments
rows=$1
columns=$2
seed=$3
generation=$4
numProcs=$5
runSerial=$6
runParallel=$7

if [ $runParallel == "y" ] && [ $numProcs == "1" ]
then
  echo "Please choose more than one process"
  exit
fi

if [ $runParallel == "y" ] && [ $rows -lt $numProcs ]
then
  echo "Please choose a larger number of rows"
  exit
fi

serialFile="serial-output.txt"
parallelFile="parallel-output.txt"

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
  rm -f $serialFile
  ./serial $rows $columns $seed $generation 
  echo "Done serial"
  echo
fi

# run the parallel version, pipe the output to the relevant file
if [ $runParallel == "y" ]
then
  echo "Running parallel"
  rm -f $parallelFile
  mpirun -np $numProcs ./parallel $rows $columns $seed $generation 
  echo "Done parallel"
  echo
fi

# compare the serial and parallel outputs to VERIFY
# only compare if we want to check the parallel version
if [ $runParallel == "y" ] 
then
  DIFF=$(diff $serialFile $parallelFile)
  if [ "$DIFF" ]
  then 
    echo "The serial and parallel outputs are different!"
  else
    echo "The serial and parallel outputs are the same and correct!"
  fi
fi

# clean up
make clean