//
// auto-generated by op2.py
//

#include <math.h>
#include <cmath>
#include "const.h"

#ifndef INLINED_FUNCS_H
#define INLINED_FUNCS_H
#include "const.h"
#include "structures.h"
#define DEBUGGABLE_ABORT fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr); fflush(stdout); exit(EXIT_FAILURE);
static inline OP_FUN_PREFIX void zero_array(int nelr, double* variables)
{
    #ifdef OMP
    #pragma omp parallel for schedule(static)
    #endif
    for(int i = 0; i < nelr; i++)
    {
        variables[i] = 0.0;
    }
}
static inline OP_FUN_PREFIX void zero_edges(int nelr, edge* mx, edge* my, edge* mz, edge* p, edge* pe)
{
    #ifdef OMP
    #pragma omp parallel
    {
    #endif
        #ifdef VERITAS
            pdump_start_profile();
        #endif
        #ifdef OMP
        #pragma omp for schedule(static)
        #endif
        #ifdef SIMD
        #pragma simd vectorlength(SIMD)
        #else
        #pragma novector
        #endif
        for(int i = 0; i < nelr; i++)
        {
            mx[i].a = 0.0;
            mx[i].b = 0.0;
            my[i].a = 0.0;
            my[i].b = 0.0;
            mz[i].a = 0.0;
            mz[i].b = 0.0;
            p[i].a = 0.0;
            p[i].b = 0.0;
            pe[i].a = 0.0;
            pe[i].b = 0.0;
        }
        #ifdef VERITAS
            pdump_end_profile();
        #endif
    #ifdef OMP
    }
    #endif
}
static inline OP_FUN_PREFIX void compute_flux_contribution(
    double& density, double3& momentum, 
    double& density_energy, 
    double& pressure, 
    double3& velocity, 
    double* fc_momentum_x, 
    double* fc_momentum_y, 
    double* fc_momentum_z, 
    double* fc_density_energy)
{
    fc_momentum_x[0] = velocity.x*momentum.x + pressure;
    fc_momentum_x[1] = velocity.x*momentum.y;
    fc_momentum_x[2] = velocity.x*momentum.z;
    fc_momentum_y[0] = fc_momentum_x[1];
    fc_momentum_y[1] = velocity.y*momentum.y + pressure;
    fc_momentum_y[2] = velocity.y*momentum.z;
    fc_momentum_z[0] = fc_momentum_x[2];
    fc_momentum_z[1] = fc_momentum_y[2];
    fc_momentum_z[2] = velocity.z*momentum.z + pressure;
    double de_p = density_energy+pressure;
    fc_density_energy[0] = velocity.x*de_p;
    fc_density_energy[1] = velocity.y*de_p;
    fc_density_energy[2] = velocity.z*de_p;
}
static inline OP_FUN_PREFIX double compute_speed_sqd(double3& velocity)
{
    return velocity.x*velocity.x + velocity.y*velocity.y + velocity.z*velocity.z;
}
static inline OP_FUN_PREFIX double compute_pressure(double& density, double& density_energy, double& speed_sqd)
{
    return (double(GAMMA)-double(1.0))*(density_energy - double(0.5)*density*speed_sqd);
}
#ifdef IDIVIDE
    static inline OP_FUN_PREFIX void compute_velocity(double& inverse_density, double3& momentum, double3& velocity)
    {
        velocity.x = momentum.x*inverse_density;
        velocity.y = momentum.y*inverse_deinlined_funcs.hnsity;
        velocity.z = momentum.z*inverse_density;
    }
    static inline OP_FUN_PREFIX double compute_speed_of_sound(double& inverse_density, double& pressure)
    {
        return cl::sycl::sqrt((double(GAMMA)*pressure)*inverse_density);
    }
#else
    static OP_FUN_PREFIX inline void compute_velocity(double& density, double3& momentum, double3& velocity)
    {
        velocity.x = momentum.x / density;
        velocity.y = momentum.y / density;
        velocity.z = momentum.z / density;
    }
    static inline OP_FUN_PREFIX double compute_speed_of_sound(double& density, double& pressure)
    {
        return cl::sycl::sqrt(double(GAMMA)*pressure / density);
    }
#endif
#endif

//user function
class get_min_dt_kernel_kernel;

//host stub function
void op_par_loop_get_min_dt_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1){

  double*arg1h = (double *)arg1.data;
  int nargs = 2;
  op_arg args[2];

  args[0] = arg0;
  args[1] = arg1;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(7);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[7].name      = name;
  OP_kernels[7].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  get_min_dt_kernel\n");
  }

  op_gpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    //set SYCL execution parameters
    #ifdef OP_BLOCK_SIZE_7
      int nthread = OP_BLOCK_SIZE_7;
    #else
      int nthread = OP_block_size;
    #endif

    int nblocks = 200;

    if (op2_queue->get_device().is_cpu()) {
      nthread = SIMD_VEC;
      nblocks = op2_queue->get_device().get_info<cl::sycl::info::device::max_compute_units>();
    }
    //transfer global reduction data to GPU
    int maxblocks = nblocks;
    int reduct_bytes = 0;
    int reduct_size  = 0;
    reduct_bytes += ROUND_UP(maxblocks*1*sizeof(double));
    reduct_size   = MAX(reduct_size,sizeof(double));
    allocReductArrays(reduct_bytes, "double");
    reduct_bytes = 0;
    arg1.data   = OP_reduct_h + reduct_bytes;
    int arg1_offset = reduct_bytes/sizeof(double);
    for ( int b=0; b<maxblocks; b++ ){
      for ( int d=0; d<1; d++ ){
        ((double *)arg1.data)[d+b*1] = arg1h[d];
      }
    }
    reduct_bytes += ROUND_UP(maxblocks*1*sizeof(double));
    mvReductArraysToDevice(reduct_bytes, "double");
    cl::sycl::buffer<double,1> *reduct = static_cast<cl::sycl::buffer<double,1> *>((void*)OP_reduct_d);

    cl::sycl::buffer<double,1> *arg0_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg0.data_d);
    int set_size = set->size+set->exec_size;
    try {
    op2_queue->submit([&](cl::sycl::handler& cgh) {
      auto arg0 = (*arg0_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
      auto reduct1 = (*reduct).template get_access<cl::sycl::access::mode::read_write>(cgh);
      cl::sycl::accessor<double, 1, cl::sycl::access::mode::read_write,
         cl::sycl::access::target::local> red_double(nthread, cgh);

      //user fun as lambda
      auto get_min_dt_kernel_gpu = [=]( 
            const double* dt,
            double* min_dt) {
            if ((*dt) < (*min_dt)) {
                *min_dt = (*dt);
            }
        
        };
        
      auto kern = [=](cl::sycl::nd_item<1> item) [[intel::reqd_sub_group_size(SIMD_VEC)]] {
        double arg1_l[1];
        for ( int d=0; d<1; d++ ){
          arg1_l[d]=reduct1[arg1_offset+d+item.get_group_linear_id()*1];
        }

        //process set elements
        for ( int n=item.get_global_linear_id(); n<set_size; n+=item.get_global_range()[0] ){

          //user-supplied kernel call
          get_min_dt_kernel_gpu(&arg0[n*1],
                                arg1_l);
        }

        //global reductions

        for ( int d=0; d<1; d++ ){
          op_reduction<OP_MIN,1>(reduct1,arg1_offset+d+item.get_group_linear_id()*1,arg1_l[d],red_double,item);
        }

      };
      cgh.parallel_for<class get_min_dt_kernel_kernel>(cl::sycl::nd_range<1>(nthread*nblocks,nthread), kern);
    });
    }catch(cl::sycl::exception const &e) {
    std::cout << e.what() << std::endl;exit(-1);
    }
    //transfer global reduction data back to CPU
    mvReductArraysToHost(reduct_bytes, "double");
    for ( int b=0; b<maxblocks; b++ ){
      for ( int d=0; d<1; d++ ){
        arg1h[d] = MIN(arg1h[d],((double *)arg1.data)[d+b*1]);
      }
    }
    arg1.data = (char *)arg1h;
    op_gpi_reduce(&arg1,arg1h);
    freeReductArrays("double");
  }
  op_gpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[7].time     += wall_t2 - wall_t1;
  OP_kernels[7].transfer += (float)set->size * arg0.size;
}
