//
// auto-generated by op2.py
//

#ifdef PAPI
#include <papi.h>
#endif

//user function
__device__ void unstructured_stream_kernel_gpu( 
    const double *variables_a,
    const double *variables_b,
    const double *edge_weight,
    double *fluxes_a,
    double *fluxes_b) {
    double ex = edge_weight[0];
    double ey = edge_weight[1];
    double ez = edge_weight[2];

    double p_a, pe_a;
    double3 momentum_a;
    p_a          = variables_a[VAR_DENSITY];
    momentum_a.x = variables_a[VAR_MOMENTUM+0];
    momentum_a.y = variables_a[VAR_MOMENTUM+1];
    momentum_a.z = variables_a[VAR_MOMENTUM+2];
    pe_a         = variables_a[VAR_DENSITY_ENERGY];

    double p_b, pe_b;
    double3 momentum_b;
    p_b          = variables_b[VAR_DENSITY];
    momentum_b.x = variables_b[VAR_MOMENTUM+0];
    momentum_b.y = variables_b[VAR_MOMENTUM+1];
    momentum_b.z = variables_b[VAR_MOMENTUM+2];
    pe_b         = variables_b[VAR_DENSITY_ENERGY];

    double p_a_val  = p_b + ex;
    double pe_a_val = pe_b + ey;
    double mx_a_val = momentum_b.x + ez;
    double my_a_val = momentum_b.y;
    double mz_a_val = momentum_b.z;

    double p_b_val = p_a;
    double pe_b_val = pe_a;
    double mx_b_val = momentum_a.x;
    double my_b_val = momentum_a.y;
    double mz_b_val = momentum_a.z;

    fluxes_a[VAR_DENSITY]  += p_a_val;
    fluxes_a[VAR_MOMENTUM+0] += mx_a_val;
    fluxes_a[VAR_MOMENTUM+1] += my_a_val;
    fluxes_a[VAR_MOMENTUM+2] += mz_a_val;
    fluxes_a[VAR_DENSITY_ENERGY] += pe_a_val;

    fluxes_b[VAR_DENSITY]  += p_b_val;
    fluxes_b[VAR_MOMENTUM+0] += mx_b_val;
    fluxes_b[VAR_MOMENTUM+1] += my_b_val;
    fluxes_b[VAR_MOMENTUM+2] += mz_b_val;
    fluxes_b[VAR_DENSITY_ENERGY] += pe_b_val;

}

// CUDA kernel function
__global__ void op_cuda_unstructured_stream_kernel(
  const double *__restrict ind_arg0,
  double *__restrict ind_arg1,
  const int *__restrict opDat0Map,
  const double *__restrict arg2,
  int    block_offset,
  int   *blkmap,
  int   *offset,
  int   *nelems,
  int   *ncolors,
  int   *colors,
  int   nblocks,
  int   set_size) {
  double arg3_l[5];
  double arg4_l[5];

  __shared__ int    nelems2, ncolor;
  __shared__ int    nelem, offset_b;

  extern __shared__ char shared[];

  if (blockIdx.x+blockIdx.y*gridDim.x >= nblocks) {
    return;
  }
  if (threadIdx.x==0) {

    //get sizes and shift pointers and direct-mapped data

    int blockId = blkmap[blockIdx.x + blockIdx.y*gridDim.x  + block_offset];

    nelem    = nelems[blockId];
    offset_b = offset[blockId];

    nelems2  = blockDim.x*(1+(nelem-1)/blockDim.x);
    ncolor   = ncolors[blockId];

  }
  __syncthreads(); // make sure all of above completed

  for ( int n=threadIdx.x; n<nelems2; n+=blockDim.x ){
    int col2 = -1;
    int map0idx;
    int map1idx;
    if (n<nelem) {
      //initialise local variables
      for ( int d=0; d<5; d++ ){
        arg3_l[d] = ZERO_double;
      }
      for ( int d=0; d<5; d++ ){
        arg4_l[d] = ZERO_double;
      }
      map0idx = opDat0Map[n + offset_b + set_size * 0];
      map1idx = opDat0Map[n + offset_b + set_size * 1];


      //user-supplied kernel call
      unstructured_stream_kernel_gpu(ind_arg0+map0idx*5,
                               ind_arg0+map1idx*5,
                               arg2+(n+offset_b)*3,
                               arg3_l,
                               arg4_l);
      col2 = colors[n+offset_b];
    }

    //store local variables

    for ( int col=0; col<ncolor; col++ ){
      if (col2==col) {
        arg3_l[0] += ind_arg1[0+map0idx*5];
        arg3_l[1] += ind_arg1[1+map0idx*5];
        arg3_l[2] += ind_arg1[2+map0idx*5];
        arg3_l[3] += ind_arg1[3+map0idx*5];
        arg3_l[4] += ind_arg1[4+map0idx*5];
        ind_arg1[0+map0idx*5] = arg3_l[0];
        ind_arg1[1+map0idx*5] = arg3_l[1];
        ind_arg1[2+map0idx*5] = arg3_l[2];
        ind_arg1[3+map0idx*5] = arg3_l[3];
        ind_arg1[4+map0idx*5] = arg3_l[4];
        arg4_l[0] += ind_arg1[0+map1idx*5];
        arg4_l[1] += ind_arg1[1+map1idx*5];
        arg4_l[2] += ind_arg1[2+map1idx*5];
        arg4_l[3] += ind_arg1[3+map1idx*5];
        arg4_l[4] += ind_arg1[4+map1idx*5];
        ind_arg1[0+map1idx*5] = arg4_l[0];
        ind_arg1[1+map1idx*5] = arg4_l[1];
        ind_arg1[2+map1idx*5] = arg4_l[2];
        ind_arg1[3+map1idx*5] = arg4_l[3];
        ind_arg1[4+map1idx*5] = arg4_l[4];
      }
      __syncthreads();
    }
  }
}


//host stub function
void op_par_loop_unstructured_stream_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){
  
  op_par_loop_unstructured_stream_kernel_instrumented(name, set, 
    arg0, arg1, arg2, arg3, arg4
    #ifdef PAPI
    , NULL
    #endif
    );
};

void op_par_loop_unstructured_stream_kernel_instrumented(
  char const *name, op_set set,
  op_arg arg0, op_arg arg1, op_arg arg2, op_arg arg3, op_arg arg4
  #ifdef PAPI
  , long_long** restrict event_counts
  #endif
  )
{

  int nargs = 5;
  op_arg args[5];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(12);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[12].name      = name;
  OP_kernels[12].count    += 1;


  int    ninds   = 2;
  int    inds[5] = {0,0,-1,1,1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: unstructured_stream_kernel\n");
  }

  //get plan
  #ifdef OP_PART_SIZE_12
    int part_size = OP_PART_SIZE_12;
  #else
    int part_size = OP_part_size;
  #endif

  int set_size = op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set_size > 0) {

    op_plan *Plan = op_plan_get(name,set,part_size,nargs,args,ninds,inds);

    //execute plan

    int block_offset = 0;
    for ( int col=0; col<Plan->ncolors; col++ ){
      if (col==Plan->ncolors_core) {
        op_mpi_wait_all_cuda(nargs, args);
      }
      #ifdef OP_BLOCK_SIZE_12
      int nthread = OP_BLOCK_SIZE_12;
      #else
      int nthread = OP_block_size;
      #endif

      dim3 nblocks = dim3(Plan->ncolblk[col] >= (1<<16) ? 65535 : Plan->ncolblk[col],
      Plan->ncolblk[col] >= (1<<16) ? (Plan->ncolblk[col]-1)/65535+1: 1, 1);
      if (Plan->ncolblk[col] > 0) {
        op_cuda_unstructured_stream_kernel<<<nblocks,nthread>>>(
        (double *)arg0.data_d,
        (double *)arg3.data_d,
        arg0.map_data_d,
        (double*)arg2.data_d,
        block_offset,
        Plan->blkmap,
        Plan->offset,
        Plan->nelems,
        Plan->nthrcol,
        Plan->thrcol,
        Plan->ncolblk[col],
        set->size+set->exec_size);

      }
      block_offset += Plan->ncolblk[col];
    }
    OP_kernels[12].transfer  += Plan->transfer;
    OP_kernels[12].transfer2 += Plan->transfer2;
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  cutilSafeCall(cudaDeviceSynchronize());
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[12].time     += wall_t2 - wall_t1;
}
