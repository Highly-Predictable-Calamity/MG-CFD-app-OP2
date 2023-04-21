#!/usr/bin/bash

module load cs402-mpi

export OP2_COMPILER=gnu
export PATH=$PATH:$HOME/installs/KaHIP-3.10/build/parallel/parallel_src/
export OP2_INSTALL_PATH=$HOME/Documents/HPC/PGAS-OP2-Common/op2
export HDF5_INSTALL_PATH=$HOME/local/bin/hdf5
export GPI_INSTALL_PATH=$HOME/local
export PARMETIS_INSTALL_PATH=$HOME/local
export KAHIP_INSTALL_PATH=$HOME/installs/KaHIP-3.10
export PTSCOTCH_INSTALL_PATH=$HOME/installs/scotch-v7.0.3