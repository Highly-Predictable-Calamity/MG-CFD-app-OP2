//
// auto-generated by op2.py
//

#include <math.h>
#include "const.h"

//user function
class up_pre_kernel_kernel;

//host stub function
void op_par_loop_up_pre_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1){

  int nargs = 2;
  op_arg args[2];

  args[0] = arg0;
  args[1] = arg1;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(16);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[16].name      = name;
  OP_kernels[16].count    += 1;


  int    ninds   = 2;
  int    inds[2] = {0,1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: up_pre_kernel\n");
  }

  //get plan
  #ifdef OP_PART_SIZE_16
    int part_size = OP_PART_SIZE_16;
  #else
    int part_size = OP_part_size;
  #endif

  op_gpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    op_plan *Plan = op_plan_get_stage(name,set,part_size,nargs,args,ninds,inds,OP_STAGE_ALL);

    cl::sycl::buffer<double,1> *arg0_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg0.data_d);
    cl::sycl::buffer<int,1> *arg1_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)arg1.data_d);
    cl::sycl::buffer<int,1> *map0_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)arg0.map_data_d);
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
        op_gpi_wait_all_cuda(nargs, args);
      }
      int nthread = SIMD_VEC;

      int nblocks = op2_queue->get_device().get_info<cl::sycl::info::device::max_compute_units>();
      int nblocks2 = Plan->ncolblk[col];
      if (Plan->ncolblk[col] > 0) {
        try {
        op2_queue->submit([&](cl::sycl::handler& cgh) {
          auto ind_arg0 = (*arg0_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto ind_arg1 = (*arg1_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
          auto opDat0Map =  (*map0_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto blkmap    = (*blkmap_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto offset    = (*offset_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto nelems    = (*nelems_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto ncolors   = (*ncolors_buffer).template get_access<cl::sycl::access::mode::read>(cgh);
          auto colors    = (*colors_buffer).template get_access<cl::sycl::access::mode::read>(cgh);



          //user fun as lambda
          auto up_pre_kernel_gpu = [=]( 
                double* variable,
                int* up_scratch) {
                variable[VAR_DENSITY] = 0.0;
                variable[VAR_MOMENTUM+0] = 0.0;
                variable[VAR_MOMENTUM+1] = 0.0;
                variable[VAR_MOMENTUM+2] = 0.0;
                variable[VAR_DENSITY_ENERGY] = 0.0;
                *up_scratch = 0;
            
            };
            
          auto kern = [=](cl::sycl::nd_item<1> item) [[intel::reqd_sub_group_size(SIMD_VEC)]] {


            //get sizes and shift pointers and direct-mapped data

            int blocksPerWG = (nblocks2-1)/item.get_group_range(0)+1;
            for ( int idx=item.get_group_linear_id()*blocksPerWG; idx<(item.get_group_linear_id()+1)*blocksPerWG && idx < nblocks2; idx++ ){
              int blockId = blkmap[idx + block_offset];

              int nelem    = nelems[blockId];
              int offset_b = offset[blockId];
              sycl::ONEAPI::sub_group sg = item.get_sub_group();


              for ( int n=item.get_local_id(0); n<nelem; n+=item.get_local_range()[0] ){
                int map0idx;
                map0idx = opDat0Map[n + offset_b + set_size * 0];


                //user-supplied kernel call
                up_pre_kernel_gpu(&ind_arg0[map0idx*5],
                                  &ind_arg1[map0idx*1]);
              }

            }
          };
          cgh.parallel_for<class up_pre_kernel_kernel>(cl::sycl::nd_range<1>(nthread*nblocks,nthread), kern);
        });
        }catch(cl::sycl::exception const &e) {
        std::cout << e.what() << std::endl;exit(-1);
        }

      }
      block_offset += Plan->ncolblk[col];
    }
    OP_kernels[16].transfer  += Plan->transfer;
    OP_kernels[16].transfer2 += Plan->transfer2;
  }
  op_gpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[16].time     += wall_t2 - wall_t1;
}
