//
// auto-generated by op2.py
//

#include <math.h>
#include <cmath>
#include "const.h"
#include "inlined_funcs.h"

//user function
__device__ void get_min_dt_kernel_gpu( 
    const double* dt,
    double* min_dt) {
    if ((*dt) < (*min_dt)) {
        *min_dt = (*dt);
    }

}

// CUDA kernel function
__global__ void op_cuda_get_min_dt_kernel(
  const double *__restrict arg0,
  double *arg1,
  int   set_size ) {

  double arg1_l[1];
  for ( int d=0; d<1; d++ ){
    arg1_l[d]=arg1[d+blockIdx.x*1];
  }

  //process set elements
  for ( int n=threadIdx.x+blockIdx.x*blockDim.x; n<set_size; n+=blockDim.x*gridDim.x ){

    //user-supplied kernel call
    get_min_dt_kernel_gpu(arg0+n*1,
                      arg1_l);
  }

  //global reductions

  for ( int d=0; d<1; d++ ){
    // op_reduction<OP_MIN>(&arg1[d+blockIdx.x*1],arg1_l[d]);
  }
}


//host stub function
void op_par_loop_get_min_dt_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1){

  double*arg1h = (double *)arg1.data;
  int nargs = 2;
  op_arg args[2];
  const int nk = 7;

  args[0] = arg0;
  args[1] = arg1;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(nk);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[nk].name      = name;
  OP_kernels[nk].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  get_min_dt_kernel");
  }

  op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    //set CUDA execution parameters
    #ifdef OP_BLOCK_SIZE_7
      int nthread = OP_BLOCK_SIZE_7;
    #else
      int nthread = OP_block_size;
    //  int nthread = 128;
    #endif

    int nblocks = 200;

    //transfer global reduction data to GPU
    int maxblocks = nblocks;
    int reduct_bytes = 0;
    int reduct_size  = 0;
    reduct_bytes += ROUND_UP(maxblocks*1*sizeof(double));
    reduct_size   = MAX(reduct_size,sizeof(double));
    reallocReductArrays(reduct_bytes);
    reduct_bytes = 0;
    arg1.data   = OP_reduct_h + reduct_bytes;
    arg1.data_d = OP_reduct_d + reduct_bytes;
    for ( int b=0; b<maxblocks; b++ ){
      for ( int d=0; d<1; d++ ){
        ((double *)arg1.data)[d+b*1] = arg1h[d];
      }
    }
    reduct_bytes += ROUND_UP(maxblocks*1*sizeof(double));
    mvReductArraysToDevice(reduct_bytes);

    int nshared = reduct_size*nthread;
    op_cuda_get_min_dt_kernel<<<nblocks,nthread,nshared>>>(
      (double *) arg0.data_d,
      (double *) arg1.data_d,
      set->size );
    //transfer global reduction data back to CPU
    mvReductArraysToHost(reduct_bytes);
    for ( int b=0; b<maxblocks; b++ ){
      for ( int d=0; d<1; d++ ){
        arg1h[d] = MIN(arg1h[d],((double *)arg1.data)[d+b*1]);
      }
    }
    arg1.data = (char *)arg1h;
    op_mpi_reduce(&arg1,arg1h);
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  cutilSafeCall(cudaDeviceSynchronize());
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[nk].time     += wall_t2 - wall_t1;
  OP_kernels[nk].transfer += (float)set->size * arg0.size;
}
