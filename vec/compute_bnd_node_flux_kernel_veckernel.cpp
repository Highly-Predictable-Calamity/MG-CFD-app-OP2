//
// auto-generated by op2.py
//

//user function
// Copyright 2009, Andrew Corrigan, acorriga@gmu.edu
// This code is from the AIAA-2009-4001 paper

#ifndef FLUX_H
#define FLUX_H

#include <math.h>

#include "inlined_funcs.h"

#include "global.h"
#include "config.h"

inline void compute_bnd_node_flux_kernel(
  const int *g, 
  const double *edge_weight, 
  const double *variables_b, 
  double *fluxes_b)
{
  // if (conf.legacy_mode) {
  //   if (mesh_name == MESH_LA_CASCADE && ((*g)==0 || (*g)==1 || (*g)==2)) {
  //     #include "flux_boundary.elem_func"
  //   } else if (mesh_name == MESH_LA_CASCADE && ((*g)==3 || (*g)==4 || (*g)==5 || (*g)==6)) {
  //     #include "flux_wall.elem_func"
  //   }
  // }

  // else {
    if ((*g) <= 2) {
      // Physical surface.
      #include "flux_boundary.elem_func"
    // } else {
    } else if ((*g) == 3 || ((*g) >= 4 && (*g) <= 7) ) {
      // g==3 => Freestream. Treat as far field.
      // g in range [4,7] => in/out sub/supersonic flow. Also treat as far field.
      #include "flux_wall.elem_func"
    }
  // }
}

inline void compute_flux_edge_kernel(
    const double *variables_a,
    const double *variables_b,
    const double *edge_weight,
    double *fluxes_a, 
    double *fluxes_b)
{
  double ewt = std::sqrt(edge_weight[0]*edge_weight[0] +
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
  double speed_b = std::sqrt(speed_sqd_b);

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

  //a
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
  double speed_a = std::sqrt(speed_sqd_a);
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

  //b
  factor_a = -ewt*smoothing_coefficient*0.5
             *(speed_a + std::sqrt(speed_sqd_b)
             + speed_of_sound_a + speed_of_sound_b);

  factor_b = -ewt*smoothing_coefficient*0.5
             *(speed_b + std::sqrt(speed_sqd_a)
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
}

#endif
#ifdef VECTORIZE
//user function -- modified for vectorisation
inline void compute_bnd_node_flux_kernel_vec( const int *g, const double *edge_weight, const double variables_b[*][SIMD_VEC], double fluxes_b[*][SIMD_VEC], int idx ) {








    if ((*g) <= 2) {

      
      
      
      
      
      
      
      
          double p_b = variables_b[VAR_DENSITY][idx];
      
          #ifdef IDIVIDE
          double ip_b = 1.0 / p_b;
          #endif
      
          double pe_b, pressure_b;
          double3 velocity_b, momentum_b;
          double flux_contribution_i_momentum_x_b[NDIM],
                 flux_contribution_i_momentum_y_b[NDIM],
                 flux_contribution_i_momentum_z_b[NDIM],
                 flux_contribution_i_density_energy_b[NDIM];
      
          momentum_b.x = variables_b[VAR_MOMENTUM+0][idx];
          momentum_b.y = variables_b[VAR_MOMENTUM+1][idx];
          momentum_b.z = variables_b[VAR_MOMENTUM+2][idx];
          pe_b = variables_b[VAR_DENSITY_ENERGY][idx];
      
          #ifdef IDIVIDE
          compute_velocity(ip_b, momentum_b, velocity_b);
          #else
          compute_velocity(p_b, momentum_b, velocity_b);
          #endif
      
          double speed_sqd_b = compute_speed_sqd(velocity_b);
          double speed_b = std::sqrt(speed_sqd_b);
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
      
          fluxes_b[VAR_DENSITY][idx]        += 0;
          fluxes_b[VAR_MOMENTUM +0][idx]    += edge_weight[0]*pressure_b;
          fluxes_b[VAR_MOMENTUM +1][idx]    += edge_weight[1]*pressure_b;
          fluxes_b[VAR_MOMENTUM +2][idx]    += edge_weight[2]*pressure_b;
          fluxes_b[VAR_DENSITY_ENERGY][idx] += 0;
      

    } else if ((*g) == 3 || ((*g) >= 4 && (*g) <= 7) ) {


      
      
      
      
      
      
      
          double p_b = variables_b[VAR_DENSITY][idx];
      
          #ifdef IDIVIDE
          double ip_b = 1.0 / p_b;
          #endif
      
          double pe_b, pressure_b;
          double3 velocity_b, momentum_b;
          double flux_contribution_i_momentum_x_b[NDIM],
                 flux_contribution_i_momentum_y_b[NDIM],
                 flux_contribution_i_momentum_z_b[NDIM],
                 flux_contribution_i_density_energy_b[NDIM];
      
          momentum_b.x = variables_b[VAR_MOMENTUM+0][idx];
          momentum_b.y = variables_b[VAR_MOMENTUM+1][idx];
          momentum_b.z = variables_b[VAR_MOMENTUM+2][idx];
          pe_b = variables_b[VAR_DENSITY_ENERGY][idx];
      
          #ifdef IDIVIDE
          compute_velocity(ip_b, momentum_b, velocity_b);
          #else
          compute_velocity(p_b, momentum_b, velocity_b);
          #endif
      
          double speed_sqd_b = compute_speed_sqd(velocity_b);
          double speed_b = std::sqrt(speed_sqd_b);
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
      
          double factor_x = 0.5 * edge_weight[0],
                 factor_y = 0.5 * edge_weight[1],
                 factor_z = 0.5 * edge_weight[2];
      
          fluxes_b[VAR_DENSITY][idx] +=
                factor_x*(ff_variable[VAR_MOMENTUM+0] + momentum_b.x)
              + factor_y*(ff_variable[VAR_MOMENTUM+1] + momentum_b.y)
              + factor_z*(ff_variable[VAR_MOMENTUM+2] + momentum_b.z);
      
          fluxes_b[VAR_DENSITY_ENERGY][idx] += 
                factor_x*(ff_flux_contribution_density_energy[0] + flux_contribution_i_density_energy_b[0])
              + factor_y*(ff_flux_contribution_density_energy[1] + flux_contribution_i_density_energy_b[1])
              + factor_z*(ff_flux_contribution_density_energy[2] + flux_contribution_i_density_energy_b[2]);
      
          fluxes_b[VAR_MOMENTUM + 0][idx] += 
                factor_x*(ff_flux_contribution_momentum_x[0] + flux_contribution_i_momentum_x_b[0])
              + factor_y*(ff_flux_contribution_momentum_x[1] + flux_contribution_i_momentum_x_b[1])
              + factor_z*(ff_flux_contribution_momentum_x[2] + flux_contribution_i_momentum_x_b[2]);
      
          fluxes_b[VAR_MOMENTUM + 1][idx] += 
                factor_x*(ff_flux_contribution_momentum_y[0] + flux_contribution_i_momentum_y_b[0])
              + factor_y*(ff_flux_contribution_momentum_y[1] + flux_contribution_i_momentum_y_b[1])
              + factor_z*(ff_flux_contribution_momentum_y[2] + flux_contribution_i_momentum_y_b[2]);
      
          fluxes_b[VAR_MOMENTUM + 2][idx] += 
                factor_x*(ff_flux_contribution_momentum_z[0] + flux_contribution_i_momentum_z_b[0])
              + factor_y*(ff_flux_contribution_momentum_z[1] + flux_contribution_i_momentum_z_b[1])
              + factor_z*(ff_flux_contribution_momentum_z[2] + flux_contribution_i_momentum_z_b[2]);
      
    }


}
#endif

// host stub function
void op_par_loop_compute_bnd_node_flux_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3){

  int nargs = 4;
  op_arg args[4];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  //create aligned pointers for dats
  ALIGNED_int const int * __restrict__ ptr0 = (int *) arg0.data;
  __assume_aligned(ptr0,int_ALIGN);
  ALIGNED_double const double * __restrict__ ptr1 = (double *) arg1.data;
  __assume_aligned(ptr1,double_ALIGN);
  ALIGNED_double const double * __restrict__ ptr2 = (double *) arg2.data;
  __assume_aligned(ptr2,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr3 = (double *) arg3.data;
  __assume_aligned(ptr3,double_ALIGN);

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(10);
  op_timers_core(&cpu_t1, &wall_t1);

  if (OP_diags>2) {
    printf(" kernel routine with indirection: compute_bnd_node_flux_kernel\n");
  }

  int exec_size = op_mpi_halo_exchanges(set, nargs, args);

  if (exec_size >0) {

    #ifdef VECTORIZE
    #pragma novector
    for ( int n=0; n<(exec_size/SIMD_VEC)*SIMD_VEC; n+=SIMD_VEC ){
      if (n+SIMD_VEC >= set->core_size) {
        op_mpi_wait_all(nargs, args);
      }
      ALIGNED_double double dat2[5][SIMD_VEC];
      ALIGNED_double double dat3[5][SIMD_VEC];
      #pragma omp simd simdlen(SIMD_VEC)
      for ( int i=0; i<SIMD_VEC; i++ ){
        int idx2_5 = 5 * arg2.map_data[(n+i) * arg2.map->dim + 0];

        dat2[0][i] = (ptr2)[idx2_5 + 0];
        dat2[1][i] = (ptr2)[idx2_5 + 1];
        dat2[2][i] = (ptr2)[idx2_5 + 2];
        dat2[3][i] = (ptr2)[idx2_5 + 3];
        dat2[4][i] = (ptr2)[idx2_5 + 4];

        dat3[0][i] = 0.0;
        dat3[1][i] = 0.0;
        dat3[2][i] = 0.0;
        dat3[3][i] = 0.0;
        dat3[4][i] = 0.0;

      }
      #pragma omp simd simdlen(SIMD_VEC)
      for ( int i=0; i<SIMD_VEC; i++ ){
        compute_bnd_node_flux_kernel_vec(
          &(ptr0)[1 * (n+i)],
          &(ptr1)[3 * (n+i)],
          dat2,
          dat3,
          i);
      }
      for ( int i=0; i<SIMD_VEC; i++ ){
        int idx3_5 = 5 * arg2.map_data[(n+i) * arg2.map->dim + 0];

        (ptr3)[idx3_5 + 0] += dat3[0][i];
        (ptr3)[idx3_5 + 1] += dat3[1][i];
        (ptr3)[idx3_5 + 2] += dat3[2][i];
        (ptr3)[idx3_5 + 3] += dat3[3][i];
        (ptr3)[idx3_5 + 4] += dat3[4][i];

      }
    }

    //remainder
    for ( int n=(exec_size/SIMD_VEC)*SIMD_VEC; n<exec_size; n++ ){
    #else
    for ( int n=0; n<exec_size; n++ ){
    #endif
      if (n==set->core_size) {
        op_mpi_wait_all(nargs, args);
      }
      int map2idx = arg2.map_data[n * arg2.map->dim + 0];

      compute_bnd_node_flux_kernel(
        &(ptr0)[1 * n],
        &(ptr1)[3 * n],
        &(ptr2)[5 * map2idx],
        &(ptr3)[5 * map2idx]);
    }
  }

  if (exec_size == 0 || exec_size == set->core_size) {
    op_mpi_wait_all(nargs, args);
  }
  // combine reduction data
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[10].name      = name;
  OP_kernels[10].count    += 1;
  OP_kernels[10].time     += wall_t2 - wall_t1;
  OP_kernels[10].transfer += (float)set->size * arg2.size;
  OP_kernels[10].transfer += (float)set->size * arg3.size * 2.0f;
  OP_kernels[10].transfer += (float)set->size * arg0.size;
  OP_kernels[10].transfer += (float)set->size * arg1.size;
  OP_kernels[10].transfer += (float)set->size * arg2.map->dim * 4.0f;
}
