#!/bin/bash
# These lines are for slurm

# specify a partition by its name in which to run your job. 
#SBATCH -p batch
# specify number of nodes you are requesting to run your job 
#SBATCH -N 1
# specify number of tasks/processes
##SBATCH -n 8
# specify the job name 
#SBATCH -J game-of-life

# specify the filename to be used for writing output.
# You need to put the correct path and the output file name. %N is name of node; %j is job number
#SBATCH -o /home-mscluster/whill/pc/project/game-of-life/output/%N.%j.out 
# specify the filename for stderr
#SBATCH -e /home-mscluster/whill/pc/project/game-of-life/error/%N.%j.err

# The following are shell commands -- which will be executed according to the settings specified above

# print extra info about the job and its execution state
echo ----------------------------------------------------------------
echo DATE: $(date)
echo Job is running on nodes $SLURM_JOB_NODELIST
echo ----------------------------------------------------------------
echo SLURM: sbatch is running $SLURM_SUBMIT_HOST
echo SLURM: job ID is $SLURM_JOB_ID
echo SLURM: submit directory is $SLURM_SUBMIT_DIR
echo SLURM: number of nodes allocated is $SLURM_JOB_NUM_NODES
# echo SLURM: number of tasks is $SLURM_NTASKS
#echo SLURM: number of cores is $SLURM_CPUS_PER_TASK
echo SLURM: job name is $SLURM_JOB_NAME
echo ----------------------------------------------------------------
cd $SLURM_SUBMIT_DIR

# run the scripts

# declare arrays
declare -a dimensions=(400 800 1600 3200 4800 6400)
declare -a numProcesses=(2 4 6 8 10 12 14 16 18 20)
generations=50

for dimensionOne in "${dimensions[@]}"
do
  for dimensionTwo in "${dimensions[@]}"
  do
    seed=$(($dimensionOne * $dimensionTwo))
    echo ""
    echo "---------------------- Board <rows, columns>: ${dimensionOne} x ${dimensionTwo} ----------------------" 
    echo "Seed: ${seed}"
    ./run.sh ${dimensionOne} ${dimensionTwo} ${seed} ${generations} 1 y n
    echo ""
    for numProcess in "${numProcesses[@]}"
    do
      echo "Number of processes: ${numProcess} ----------------------"
      ./run.sh ${dimensionOne} ${dimensionTwo} ${seed} ${generations} ${numProcess} n y
      echo ""
    done
  done
done

echo ""
echo "---------- Done! ----------"
