//
// auto-generated by op2.py
//

#include <cmath>
#include "const.h"
#include "structures.h"
#include "global.h"

//user function
__device__ void zero_1d_array_kernel_gpu( 
    double* array) {
    *array = 0.0;

}

// CUDA kernel function
__global__ void op_cuda_zero_1d_array_kernel(
  double *arg0,
  int   set_size ) {


  //process set elements
  for ( int n=threadIdx.x+blockIdx.x*blockDim.x; n<set_size; n+=blockDim.x*gridDim.x ){

    //user-supplied kernel call
    zero_1d_array_kernel_gpu(arg0+n*1);
  }
}


//host stub function
void op_par_loop_zero_1d_array_kernel(char const *name, op_set set,
  op_arg arg0){

  int nargs = 1;
  op_arg args[1];

  args[0] = arg0;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(2);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[2].name      = name;
  OP_kernels[2].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  zero_1d_array_kernel");
  }

  int set_size = op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set_size > 0) {

    //set CUDA execution parameters
    #ifdef OP_BLOCK_SIZE_2
      int nthread = OP_BLOCK_SIZE_2;
    #else
      int nthread = OP_block_size;
    #endif

    int nblocks = 200;

    op_cuda_zero_1d_array_kernel<<<nblocks,nthread>>>(
      (double *) arg0.data_d,
      set->size );
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  cutilSafeCall(cudaDeviceSynchronize());
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[2].time     += wall_t2 - wall_t1;
  OP_kernels[2].transfer += (float)set->size * arg0.size * 2.0f;
}
