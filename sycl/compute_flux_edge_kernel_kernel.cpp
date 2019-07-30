//
// auto-generated by op2.py
//

#include <math.h>
#include "inlined_funcs.h"
#include "global.h"
#include "config.h"

//user function
class compute_flux_edge_kernel_kernel;

//host stub function
void op_par_loop_compute_flux_edge_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){

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

  op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    //set SYCL execution parameters
    #ifdef OP_BLOCK_SIZE_9
      int nthread = OP_BLOCK_SIZE_9;
    #else
      int nthread = OP_block_size;
    #endif

    cl::sycl::buffer<double,1> *arg0_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg0.data_d);
    cl::sycl::buffer<double,1> *arg3_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg3.data_d);
    cl::sycl::buffer<int,1> *map0_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)arg0.map_data_d);
    cl::sycl::buffer<double,1> *arg2_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg2.data_d);
    int set_size = set->size+set->exec_size;
    for ( int round=0; round<2; round++ ){
      if (round==1) {
        op_mpi_wait_all_cuda(nargs, args);
      }
      int start = round==0 ? 0 : set->core_size;
      int end = round==0 ? set->core_size : set->size + set->exec_size;
      if (end-start>0) {
        int nblocks = (end-start-1)/nthread+1;
        try {
        op2_queue->submit([&](cl::sycl::handler& cgh) {
          auto ind_arg0 = (*arg0_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto ind_arg1 = (*arg3_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto opDat0Map =  (*map0_buffer).template get_access<cl::sycl::access::mode::read>(cgh);

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
            
          auto kern = [=](cl::sycl::nd_item<1> item) {
            double arg3_l[5];
            for ( int d=0; d<5; d++ ){
              arg3_l[d] = ZERO_double;
            }
            double arg4_l[5];
            for ( int d=0; d<5; d++ ){
              arg4_l[d] = ZERO_double;
            }
            int tid = item.get_global_linear_id();
            if (tid + start < end) {
              int n = tid+start;
              //initialise local variables
              int map0idx;
              int map1idx;
              map0idx = opDat0Map[n + set_size * 0];
              map1idx = opDat0Map[n + set_size * 1];

              //user-supplied kernel call
              compute_flux_edge_kernel_gpu(&ind_arg0[map0idx*5],
                             &ind_arg0[map1idx*5],
                             &arg2[n*3],
                             arg3_l,
                             arg4_l);
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[0+map0idx*5]}}; a.fetch_add(arg3_l[0]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[1+map0idx*5]}}; a.fetch_add(arg3_l[1]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[2+map0idx*5]}}; a.fetch_add(arg3_l[2]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[3+map0idx*5]}}; a.fetch_add(arg3_l[3]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[4+map0idx*5]}}; a.fetch_add(arg3_l[4]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[0+map1idx*5]}}; a.fetch_add(arg4_l[0]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[1+map1idx*5]}}; a.fetch_add(arg4_l[1]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[2+map1idx*5]}}; a.fetch_add(arg4_l[2]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[3+map1idx*5]}}; a.fetch_add(arg4_l[3]);}
              {cl::sycl::atomic<double> a{cl::sycl::global_ptr<double>{&ind_arg1[4+map1idx*5]}}; a.fetch_add(arg4_l[4]);}
            }

          };
          cgh.parallel_for<class compute_flux_edge_kernel_kernel>(cl::sycl::nd_range<1>(nthread*nblocks,nthread), kern);
        });
        }catch(cl::sycl::exception const &e) {
        std::cout << e.what() << std::endl;exit(-1);
        }

      }
    }
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[9].time     += wall_t2 - wall_t1;
}
