//
// auto-generated by op2.py
//

//user function
#include ".././src/Kernels/flux.h"

#ifdef PAPI
#include "papi_funcs.h"
#endif

// host stub function
void op_par_loop_compute_flux_edge_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){
  
  op_par_loop_compute_flux_edge_kernel_instrumented(name, set, 
    arg0, arg1, arg2, arg3, arg4
    #ifdef VERIFY_OP2_TIMING
      , NULL, NULL
    #endif
    , NULL
    #ifdef PAPI
    , NULL
    #endif
    );
};

void op_par_loop_compute_flux_edge_kernel_instrumented(
  char const *name, op_set set,
  op_arg arg0, op_arg arg1, op_arg arg2, op_arg arg3, op_arg arg4
  #ifdef VERIFY_OP2_TIMING
    , double* compute_time_ptr, double* sync_time_ptr
  #endif
  , long* iter_counts_ptr
  #ifdef PAPI
  , long_long** event_counts
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
  op_timing_realloc(9);
  op_timers_core(&cpu_t1, &wall_t1);
  double inner_cpu_t1, inner_cpu_t2, inner_wall_t1, inner_wall_t2;
  double compute_time=0.0, sync_time=0.0;
  long iter_counts=0;

  if (OP_diags>2) {
    printf(" kernel routine with indirection: compute_flux_edge_kernel\n");
  }

  op_timers_core(&inner_cpu_t1, &inner_wall_t1);
  int set_size = op_gpi_halo_exchanges(set, nargs, args);
  op_timers_core(&inner_cpu_t2, &inner_wall_t2);
  sync_time += inner_wall_t2 - inner_wall_t1;

  if (set_size >0) {

    #ifdef PAPI
      my_papi_start();
    #endif

    op_timers_core(&inner_cpu_t1, &inner_wall_t1);
    for ( int n=0; n<set_size; n++ ){
      if (n==set->core_size) {
        #ifdef PAPI
          my_papi_stop(event_counts);
        #endif

        op_timers_core(&inner_cpu_t2, &inner_wall_t2);
        compute_time += inner_wall_t2 - inner_wall_t1;
        op_gpi_waitall_args(nargs, args);
        op_timers_core(&inner_cpu_t1, &inner_wall_t1);
        sync_time += inner_wall_t1 - inner_wall_t2;

        #ifdef PAPI
          my_papi_start();
        #endif
      }
      int map0idx = arg0.map_data[n * arg0.map->dim + 0];
      int map1idx = arg0.map_data[n * arg0.map->dim + 1];


      compute_flux_edge_kernel(
        &((double*)arg0.data)[5 * map0idx],
        &((double*)arg0.data)[5 * map1idx],
        &((double*)arg2.data)[3 * n],
        &((double*)arg3.data)[5 * map0idx],
        &((double*)arg3.data)[5 * map1idx]);
    }
    op_timers_core(&inner_cpu_t2, &inner_wall_t2);
    compute_time += inner_wall_t2 - inner_wall_t1;
    iter_counts += set_size;

    #ifdef PAPI
      my_papi_stop(event_counts);
    #endif
  }

  op_timers_core(&inner_cpu_t1, &inner_wall_t1);
  if (set_size == 0 || set_size == set->core_size) {
    op_gpi_waitall_args(nargs, args);
  }
  // combine reduction data
  op_gpi_set_dirtybit(nargs, args);
  op_timers_core(&inner_cpu_t2, &inner_wall_t2);
  sync_time += inner_wall_t2 - inner_wall_t1;

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[9].name      = name;
  OP_kernels[9].count    += 1;
  OP_kernels[9].time     += wall_t2 - wall_t1;
  OP_kernels[9].transfer += (float)set->size * arg0.size;
  OP_kernels[9].transfer += (float)set->size * arg3.size * 2.0f;
  OP_kernels[9].transfer += (float)set->size * arg2.size;
  OP_kernels[9].transfer += (float)set->size * arg0.map->dim * 4.0f;

  #ifdef VERIFY_OP2_TIMING
    if (compute_time_ptr != NULL)
        *compute_time_ptr += compute_time;
    if (sync_time_ptr != NULL)
        *sync_time_ptr += sync_time;
  #endif
  if (iter_counts_ptr != NULL)
      *iter_counts_ptr += iter_counts;
}
