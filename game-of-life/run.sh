# make sure we have the correct arguments
if [ "$#" != 5 ]
then
  echo "We will specify the size of the game board, and provide a random seed that will be used to generate the same board for the serial and parallel versions. We will also need to specify how many generations to run the game for (-1 for indefinite):\n"
  echo "Usage: ${0} <rows> <columns> <seed> <generation> <number of processes>"
  exit
fi

# we have the correct arguments
rows=$1
columns=$2
seed=$3
generation=$4
numProcs=$5

serialFile="serial-output.txt"
parallelFile="parallel-output.txt"

# make the serial and parallel versions
make clean
make

# run the serial version, pipe the output to the relevant file
echo "Running serial"
rm -f $serialFile
./serial $rows $columns $seed $generation > $serialFile
echo "Done serial\n"

# run the parallel version, pipe the output to the relevant file
echo "Running parallel"
rm -f $parallelFile
mpirun -np $numProcs ./parallel $rows $columns $seed $generation > $parallelFile
echo "Done parallel\n"

# compare the serial and parallel outputs to VERIFY
DIFF=$(diff $serialFile $parallelFile)
if [ "$DIFF" ]
then 
  echo "The serial and parallel outputs are different!"
else
  echo "The serial and parallel outputs are the same and correct!"
fi

# clean up
make clean