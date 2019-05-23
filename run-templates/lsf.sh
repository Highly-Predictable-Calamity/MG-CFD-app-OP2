#!/bin/bash

#BSUB -J MG-CFD.<RUN ID>
#BSUB -o lsf.stdout
#BSUB -e lsf.stderr

#BSUB -nnodes <NODES>
#BSUB -R "span[ptile=<NCPUS_PER_NODE>] affinity[core(<NTHREADS>, same=socket)]"
#BSUB -W <HOURS>:<MINUTES>
#BSUB -x
#BSUB -q <PARTITION>
#BSUB -P <PROJECT CODE>

RUN_CMD="mpirun -n <NTASKS>"
