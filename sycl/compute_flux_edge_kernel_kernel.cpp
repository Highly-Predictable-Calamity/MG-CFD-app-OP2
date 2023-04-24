//
// auto-generated by op2.py
//

#include <math.h>

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
#include "global.h"
#include "config.h"

//user function
class compute_flux_edge_kernel_kernel;

#ifdef PAPI
#include "papi_funcs.h"
#endif

void op_par_loop_compute_flux_edge_kernel_instrumented(
  char const *name, op_set set,
  op_arg arg0, op_arg arg1, op_arg arg2, op_arg arg3, op_arg arg4
  #ifdef VERIFY_OP2_TIMING
    , double* compute_time_ptr, double* sync_time_ptr
  #endif
  , long* iter_counts_ptr
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
  op_timing_realloc(9);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[9].name      = name;
  OP_kernels[9].count    += 1;


  int    ninds   = 2;
  int    inds[5] = {0,0,-1,1,1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: compute_flux_edge_kernel\n");
  }

  //get plan
  #ifdef OP_PART_SIZE_9
    int part_size = OP_PART_SIZE_9;
  #else
    int part_size = OP_part_size;
  #endif

  op_gpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    op_plan *Plan = op_plan_get_stage(name,set,part_size,nargs,args,ninds,inds,OP_STAGE_ALL);

    #ifdef PAPI
      my_papi_start();
    #endif

    cl::sycl::buffer<double,1> *arg0_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg0.data_d);
    cl::sycl::buffer<double,1> *arg3_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg3.data_d);
    cl::sycl::buffer<int,1> *map0_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)arg0.map_data_d);
    cl::sycl::buffer<double,1> *arg2_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg2.data_d);
    cl::sycl::buffer<int,1> *blkmap_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)Plan->blkmap);
    cl::sycl::buffer<int,1> *offset_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)Plan->offset);
    cl::sycl::buffer<int,1> *nelems_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)Plan->nelems);
    cl::sycl::buffer<int,1> *ncolors_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)Plan->nthrcol);
    cl::sycl::buffer<int,1> *colors_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)Plan->thrcol);
    int set_size = set->size+set->exec_size;
    //execute plan

    int block_offset = 0;
    for ( int col=0; col<Plan->ncolors; col++ ){
      if (col==Plan->ncolors_core) {
        #ifdef PAPI
          my_papi_stop(event_counts);
        #endif

        op_gpi_wait_all_cuda(nargs, args);

        #ifdef PAPI
          my_papi_start();
        #endif
      }
      int nthread = SIMD_VEC;

      int nblocks = op2_queue->get_device().get_info<cl::sycl::info::device::max_compute_units>();
      int nblocks2 = Plan->ncolblk[col];
      if (Plan->ncolblk[col] > 0) {
        try {
        op2_queue->submit([&](cl::sycl::handler& cgh) {
          auto ind_arg0 = (*arg0_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto ind_arg1 = (*arg3_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto opDat0Map =  (*map0_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto blkmap    = (*blkmap_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto offset    = (*offset_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto nelems    = (*nelems_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto ncolors   = (*ncolors_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto colors    = (*colors_buffer).template get_access<cl::sycl::access::mode::read>(cgh);

          auto arg2 = (*arg2_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);

          auto smoothing_coefficient_sycl = (*smoothing_coefficient_p).template get_access<cl::sycl::access::mode::read>(cgh);

          //user fun as lambda
          auto compute_flux_edge_kernel_gpu = [=]( 
                const double *variables_a,
                const double *variables_b,
                const double *edge_weight,
                double *fluxes_a,
                double *fluxes_b) {
              double ewt = cl::sycl::sqrt(edge_weight[0]*edge_weight[0] +
                                     edge_weight[1]*edge_weight[1] +
                                     edge_weight[2]*edge_weight[2]);
            
              double p_b = variables_b[VAR_DENSITY];
            
              #ifdef IDIVIDE
              double ip_b = 1.0 / p_b;
              #endif
            
              double pe_b, pressure_b;
              double3 velocity_b, momentum_b;
              double flux_contribution_i_momentum_x_b[NDIM],
                     flux_contribution_i_momentum_y_b[NDIM],
                     flux_contribution_i_momentum_z_b[NDIM],
                     flux_contribution_i_density_energy_b[NDIM];
            
              momentum_b.x = variables_b[VAR_MOMENTUM+0];
              momentum_b.y = variables_b[VAR_MOMENTUM+1];
              momentum_b.z = variables_b[VAR_MOMENTUM+2];
              pe_b = variables_b[VAR_DENSITY_ENERGY];
            
              #ifdef IDIVIDE
              compute_velocity(ip_b, momentum_b, velocity_b);
              #else
              compute_velocity(p_b, momentum_b, velocity_b);
              #endif
            
              double speed_sqd_b = compute_speed_sqd(velocity_b);
              double speed_b = cl::sycl::sqrt(speed_sqd_b);
            
              pressure_b = compute_pressure(p_b, pe_b, speed_sqd_b);
            
              #ifdef IDIVIDE
              double speed_of_sound_b = compute_speed_of_sound(ip_b, pressure_b);
              #else
              double speed_of_sound_b = compute_speed_of_sound(p_b, pressure_b);
              #endif
            
              compute_flux_contribution(p_b, momentum_b, pe_b,
                  pressure_b, velocity_b,
                  flux_contribution_i_momentum_x_b,
                  flux_contribution_i_momentum_y_b,
                  flux_contribution_i_momentum_z_b,
                  flux_contribution_i_density_energy_b);
            
              double factor_a, factor_b;
            
              double p_a, pe_a, pressure_a;
              double3 velocity_a, momentum_a;
              double flux_contribution_i_momentum_x_a[NDIM],
                     flux_contribution_i_momentum_y_a[NDIM],
                     flux_contribution_i_momentum_z_a[NDIM],
                     flux_contribution_i_density_energy_a[NDIM];
            
              p_a = variables_a[VAR_DENSITY];
            
              #ifdef IDIVIDE
              double ip_a = 1.0 / p_a;
              #endif
            
              momentum_a.x = variables_a[VAR_MOMENTUM+0];
              momentum_a.y = variables_a[VAR_MOMENTUM+1];
              momentum_a.z = variables_a[VAR_MOMENTUM+2];
              pe_a = variables_a[VAR_DENSITY_ENERGY];
            
              #ifdef IDIVIDE
              compute_velocity(ip_a, momentum_a, velocity_a);
              #else
              compute_velocity(p_a, momentum_a, velocity_a);
              #endif
            
              double speed_sqd_a = compute_speed_sqd(velocity_a);
              double speed_a = cl::sycl::sqrt(speed_sqd_a);
              pressure_a = compute_pressure(p_a, pe_a, speed_sqd_a);
            
              #ifdef IDIVIDE
              double speed_of_sound_a = compute_speed_of_sound(ip_a, pressure_a);
              #else
              double speed_of_sound_a = compute_speed_of_sound(p_a, pressure_a);
              #endif
            
              compute_flux_contribution(p_a, momentum_a, pe_a,
                                        pressure_a, velocity_a,
                                        flux_contribution_i_momentum_x_a,
                                        flux_contribution_i_momentum_y_a,
                                        flux_contribution_i_momentum_z_a,
                                        flux_contribution_i_density_energy_a);
            
              factor_a = -ewt*smoothing_coefficient_sycl[0]*0.5
                         *(speed_a + cl::sycl::sqrt(speed_sqd_b)
                         + speed_of_sound_a + speed_of_sound_b);
            
              factor_b = -ewt*smoothing_coefficient_sycl[0]*0.5
                         *(speed_b + cl::sycl::sqrt(speed_sqd_a)
                         + speed_of_sound_b + speed_of_sound_a);
            
              double factor_x = -0.5*edge_weight[0], factor_y = -0.5*edge_weight[1], factor_z = -0.5*edge_weight[2];
            
              fluxes_a[VAR_DENSITY] +=
                  factor_a*(p_a - p_b)
                + factor_x*(momentum_a.x + momentum_b.x)
                + factor_y*(momentum_a.y + momentum_b.y)
                + factor_z*(momentum_a.z + momentum_b.z);
            
              fluxes_a[VAR_DENSITY_ENERGY] +=
                  factor_a*(pe_a - pe_b)
                + factor_x*(flux_contribution_i_density_energy_a[0] + flux_contribution_i_density_energy_b[0])
                + factor_y*(flux_contribution_i_density_energy_a[1] + flux_contribution_i_density_energy_b[1])
                + factor_z*(flux_contribution_i_density_energy_a[2] + flux_contribution_i_density_energy_b[2]);
            
              fluxes_a[VAR_MOMENTUM + 0] +=
                  factor_a*(momentum_a.x - momentum_b.x)
                + factor_x*(flux_contribution_i_momentum_x_a[0] + flux_contribution_i_momentum_x_b[0])
                + factor_y*(flux_contribution_i_momentum_x_a[1] + flux_contribution_i_momentum_x_b[1])
                + factor_z*(flux_contribution_i_momentum_x_a[2] + flux_contribution_i_momentum_x_b[2]);
            
              fluxes_a[VAR_MOMENTUM + 1] +=
                  factor_a*(momentum_a.y - momentum_b.y)
                + factor_x*(flux_contribution_i_momentum_y_a[0] + flux_contribution_i_momentum_y_b[0])
                + factor_y*(flux_contribution_i_momentum_y_a[1] + flux_contribution_i_momentum_y_b[1])
                + factor_z*(flux_contribution_i_momentum_y_a[2] + flux_contribution_i_momentum_y_b[2]);
            
              fluxes_a[VAR_MOMENTUM + 2] +=
                  factor_a*(momentum_a.z - momentum_b.z)
                + factor_x*(flux_contribution_i_momentum_z_a[0] + flux_contribution_i_momentum_z_b[0])
                + factor_y*(flux_contribution_i_momentum_z_a[1] + flux_contribution_i_momentum_z_b[1])
                + factor_z*(flux_contribution_i_momentum_z_a[2] + flux_contribution_i_momentum_z_b[2]);
            
              fluxes_b[VAR_DENSITY] +=
                  factor_b*(p_b - p_a)
                - factor_x*(momentum_a.x + momentum_b.x)
                - factor_y*(momentum_a.y + momentum_b.y)
                - factor_z*(momentum_a.z + momentum_b.z);
            
              fluxes_b[VAR_DENSITY_ENERGY] +=
                  factor_b*(pe_b - pe_a)
                - factor_x*(flux_contribution_i_density_energy_a[0] + flux_contribution_i_density_energy_b[0])
                - factor_y*(flux_contribution_i_density_energy_a[1] + flux_contribution_i_density_energy_b[1])
                - factor_z*(flux_contribution_i_density_energy_a[2] + flux_contribution_i_density_energy_b[2]);
            
              fluxes_b[VAR_MOMENTUM + 0] +=
                  factor_b*(momentum_b.x - momentum_a.x)
                - factor_x*(flux_contribution_i_momentum_x_a[0] + flux_contribution_i_momentum_x_b[0])
                - factor_y*(flux_contribution_i_momentum_x_a[1] + flux_contribution_i_momentum_x_b[1])
                - factor_z*(flux_contribution_i_momentum_x_a[2] + flux_contribution_i_momentum_x_b[2]);
            
              fluxes_b[VAR_MOMENTUM + 1] +=
                  factor_b*(momentum_b.y - momentum_a.y)
                - factor_x*(flux_contribution_i_momentum_y_a[0] + flux_contribution_i_momentum_y_b[0])
                - factor_y*(flux_contribution_i_momentum_y_a[1] + flux_contribution_i_momentum_y_b[1])
                - factor_z*(flux_contribution_i_momentum_y_a[2] + flux_contribution_i_momentum_y_b[2]);
            
              fluxes_b[VAR_MOMENTUM + 2] +=
                  factor_b*(momentum_b.z - momentum_a.z)
                - factor_x*(flux_contribution_i_momentum_z_a[0] + flux_contribution_i_momentum_z_b[0])
                - factor_y*(flux_contribution_i_momentum_z_a[1] + flux_contribution_i_momentum_z_b[1])
                - factor_z*(flux_contribution_i_momentum_z_a[2] + flux_contribution_i_momentum_z_b[2]);
            
            };
            
          auto kern = [=](cl::sycl::nd_item<1> item) [[intel::reqd_sub_group_size(SIMD_VEC)]] {
            double arg3_l[5];
            double arg4_l[5];


            //get sizes and shift pointers and direct-mapped data

            int blocksPerWG = (nblocks2-1)/item.get_group_range(0)+1;
            for ( int idx=item.get_group_linear_id()*blocksPerWG; idx<(item.get_group_linear_id()+1)*blocksPerWG && idx < nblocks2; idx++ ){
              int blockId = blkmap[idx + block_offset];

              int nelem    = nelems[blockId];
              int offset_b = offset[blockId];
              sycl::ONEAPI::sub_group sg = item.get_sub_group();

              int nelems2  = item.get_local_range()[0]*(1+(nelem-1)/item.get_local_range()[0]);
              int ncolor   = ncolors[blockId];


              for ( int n=item.get_local_id(0); n<nelems2; n+=item.get_local_range()[0] ){
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
                  compute_flux_edge_kernel_gpu(&ind_arg0[map0idx*5],
                                               &ind_arg0[map1idx*5],
                                               &arg2[(n+offset_b)*3],
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
                  sg.barrier();
                }
              }

            }
          };
          cgh.parallel_for<class compute_flux_edge_kernel_kernel>(cl::sycl::nd_range<1>(nthread*nblocks,nthread), kern);
        });
        }catch(cl::sycl::exception const &e) {
        std::cout << e.what() << std::endl;exit(-1);
        }

      }
      block_offset += Plan->ncolblk[col];
    }
    OP_kernels[9].transfer  += Plan->transfer;
    OP_kernels[9].transfer2 += Plan->transfer2;
    
    #ifdef PAPI
      my_papi_stop(event_counts);
    #endif
  }
  op_gpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[9].time     += wall_t2 - wall_t1;
}

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

