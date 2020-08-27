//
// auto-generated by op2.py
//

//user function
#ifndef MISC_H
#define MISC_H

#include <cmath>

#include "const.h"
#include "structures.h"
#include "global.h"

inline void initialize_variables_kernel(
    double* variables)
{
    for(int j = 0; j < NVAR; j++) {
        variables[j] = ff_variable[j];
    }
}

inline void zero_5d_array_kernel(
    double* array)
{
    for(int j = 0; j < NVAR; j++) {
        array[j] = 0.0;
    }
}

inline void zero_3d_array_kernel(
    double* array)
{
    for(int j = 0; j < NDIM; j++) {
        array[j] = 0.0;
    }
}

inline void zero_1d_array_kernel(
    double* array)
{
    *array = 0.0;
}

inline void calculate_cell_volumes(
    const double* coords1, 
    const double* coords2, 
    double* ewt,
    double* vol1, 
    double* vol2)
{
    double d[NDIM];
    double dist = 0.0;
    for (int i=0; i<NDIM; i++) {
        d[i] = coords2[i] - coords1[i];
        dist += d[i]*d[i];
    }
    dist = sqrt(dist);

    double area = 0.0;
    for (int i=0; i<NDIM; i++) {
        area += ewt[i]*ewt[i];
    }
    area = sqrt(area);

    double tetra_volume = (1.0/3.0)*0.5 *dist *area;
    *vol1 += tetra_volume;
    *vol2 += tetra_volume;

    // Redirect ewt to be parallel to normal:
    for (int i=0; i<NDIM; i++) {
        ewt[i] = (d[i] / dist) * area;
    }

    // |ewt| currently is face area. Divide through by distance 
    // to produce 'surface vector' with magnitude (area/dm), 
    // for use in flux accumulation:
    for (int i=0; i<NDIM; i++) {
        ewt[i] /= dist;
    }
}

inline void dampen_ewt(
    double* ewt)
{
    ewt[0] *= 1e-7;
    ewt[1] *= 1e-7;
    ewt[2] *= 1e-7;
}

#endif
#ifdef VECTORIZE
//user function -- modified for vectorisation
#if defined __clang__ || defined __GNUC__
__attribute__((always_inline))
#endif
inline void calculate_cell_volumes_vec( const double coords1[][SIMD_VEC], const double coords2[][SIMD_VEC], double ewt[][SIMD_VEC], double vol1[][SIMD_VEC], double vol2[][SIMD_VEC], int idx ) {
    double d[NDIM];
    double dist = 0.0;
    for (int i=0; i<NDIM; i++) {
        d[i] = coords2[i][idx] - coords1[i][idx];
        dist += d[i]*d[i];
    }
    dist = sqrt(dist);

    double area = 0.0;
    for (int i=0; i<NDIM; i++) {
        area += ewt[i][idx]*ewt[i][idx];
    }
    area = sqrt(area);

    double tetra_volume = (1.0/3.0)*0.5 *dist *area;
    vol1[0][idx]= tetra_volume;
    vol2[0][idx]= tetra_volume;

    for (int i=0; i<NDIM; i++) {
        ewt[i][idx] = (d[i] / dist) * area;
    }



    for (int i=0; i<NDIM; i++) {
        ewt[i][idx] /= dist;
    }

}
#endif

// host stub function
void op_par_loop_calculate_cell_volumes(char const *name, op_set set,
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
  //create aligned pointers for dats
  ALIGNED_double const double * __restrict__ ptr0 = (double *) arg0.data;
  DECLARE_PTR_ALIGNED(ptr0,double_ALIGN);
  ALIGNED_double const double * __restrict__ ptr1 = (double *) arg1.data;
  DECLARE_PTR_ALIGNED(ptr1,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr2 = (double *) arg2.data;
  DECLARE_PTR_ALIGNED(ptr2,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr3 = (double *) arg3.data;
  DECLARE_PTR_ALIGNED(ptr3,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr4 = (double *) arg4.data;
  DECLARE_PTR_ALIGNED(ptr4,double_ALIGN);

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(3);
  op_timers_core(&cpu_t1, &wall_t1);

  if (OP_diags>2) {
    printf(" kernel routine with indirection: calculate_cell_volumes\n");
  }

  int exec_size = op_mpi_halo_exchanges(set, nargs, args);

  if (exec_size >0) {

    #ifdef VECTORIZE
    #pragma novector
    for ( int n=0; n<(exec_size/SIMD_VEC)*SIMD_VEC; n+=SIMD_VEC ){
      if (n+SIMD_VEC >= set->core_size) {
        op_mpi_wait_all(nargs, args);
      }
      ALIGNED_double double dat0[3][SIMD_VEC];
      ALIGNED_double double dat1[3][SIMD_VEC];
      ALIGNED_double double dat2[3][SIMD_VEC];
      ALIGNED_double double dat3[1][SIMD_VEC];
      ALIGNED_double double dat4[1][SIMD_VEC];
      #pragma omp simd simdlen(SIMD_VEC)
      for ( int i=0; i<SIMD_VEC; i++ ){
        int idx0_3 = 3 * arg0.map_data[(n+i) * arg0.map->dim + 0];
        int idx1_3 = 3 * arg0.map_data[(n+i) * arg0.map->dim + 1];
        int idx2_3 = 3 * (n+i);

        dat0[0][i] = (ptr0)[idx0_3 + 0];
        dat0[1][i] = (ptr0)[idx0_3 + 1];
        dat0[2][i] = (ptr0)[idx0_3 + 2];

        dat1[0][i] = (ptr1)[idx1_3 + 0];
        dat1[1][i] = (ptr1)[idx1_3 + 1];
        dat1[2][i] = (ptr1)[idx1_3 + 2];

        dat2[0][i] = (ptr2)[idx2_3 + 0];
        dat2[1][i] = (ptr2)[idx2_3 + 1];
        dat2[2][i] = (ptr2)[idx2_3 + 2];

        dat3[0][i] = 0.0;

        dat4[0][i] = 0.0;

      }
      #pragma omp simd simdlen(SIMD_VEC)
      for ( int i=0; i<SIMD_VEC; i++ ){
        calculate_cell_volumes_vec(
          dat0,
          dat1,
          dat2,
          dat3,
          dat4,
          i);
      }
      for ( int i=0; i<SIMD_VEC; i++ ){
        int idx2_3 = 3 * (n+i);
        int idx3_1 = 1 * arg0.map_data[(n+i) * arg0.map->dim + 0];
        int idx4_1 = 1 * arg0.map_data[(n+i) * arg0.map->dim + 1];

        (ptr2)[idx2_3 + 0]  = dat2[0][i];
        (ptr2)[idx2_3 + 1]  = dat2[1][i];
        (ptr2)[idx2_3 + 2]  = dat2[2][i];

        (ptr3)[idx3_1 + 0] += dat3[0][i];

        (ptr4)[idx4_1 + 0] += dat4[0][i];

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
      int map0idx = arg0.map_data[n * arg0.map->dim + 0];
      int map1idx = arg0.map_data[n * arg0.map->dim + 1];

      calculate_cell_volumes(
        &(ptr0)[3 * map0idx],
        &(ptr1)[3 * map1idx],
        &(ptr2)[3 * n],
        &(ptr3)[1 * map0idx],
        &(ptr4)[1 * map1idx]);
    }
  }

  if (exec_size == 0 || exec_size == set->core_size) {
    op_mpi_wait_all(nargs, args);
  }
  // combine reduction data
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[3].name      = name;
  OP_kernels[3].count    += 1;
  OP_kernels[3].time     += wall_t2 - wall_t1;
  OP_kernels[3].transfer += (float)set->size * arg0.size;
  OP_kernels[3].transfer += (float)set->size * arg3.size * 2.0f;
  OP_kernels[3].transfer += (float)set->size * arg2.size * 2.0f;
  OP_kernels[3].transfer += (float)set->size * arg0.map->dim * 4.0f;
}
