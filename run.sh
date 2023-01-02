#!/bin/bash
#SBATCH --cpus-per-task=1   # maximum CPU cores per GPU request: 6 on Cedar, 16 on Graham.
#SBATCH --mem=16000M        # memory per node
#SBATCH --time=00-01:30      # time (DD-HH:MM)
#SBATCH --account=def-whitem
#SBATCH --array=0-49 # array index
#SBATCH --output=output/%A-%a.out  # %N for node name, %j for jobID

./main -exps