//
// auto-generated by op2.py
//

#include <math.h>
#include "const.h"

//user function
class up_post_kernel_kernel;

//host stub function
void op_par_loop_up_post_kernel(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1){

  int nargs = 2;
  op_arg args[2];

  args[0] = arg0;
  args[1] = arg1;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(18);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[18].name      = name;
  OP_kernels[18].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  up_post_kernel\n");
  }

  op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    //set SYCL execution parameters
    #ifdef OP_BLOCK_SIZE_18
      int nthread = OP_BLOCK_SIZE_18;
    #else
      int nthread = OP_block_size;
    #endif

    int nblocks = 200;

    cl::sycl::buffer<double,1> *arg0_buffer = static_cast<cl::sycl::buffer<double,1>*>((void*)arg0.data_d);
    cl::sycl::buffer<int,1> *arg1_buffer = static_cast<cl::sycl::buffer<int,1>*>((void*)arg1.data_d);
    int set_size = set->size+set->exec_size;
    try {
    op2_queue->submit([&](cl::sycl::handler& cgh) {
      auto arg0 = (*arg0_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);
      auto arg1 = (*arg1_buffer).template get_access<cl::sycl::access::mode::read_write>(cgh);

      //user fun as lambda
      auto up_post_kernel_gpu = [=]( 
            double* variable,
            const int* up_scratch) {
            double avg = (*up_scratch)==0 ? 1.0 : 1.0 / (double)(*up_scratch);
            variable[VAR_DENSITY] *= avg;
            variable[VAR_MOMENTUM+0] *= avg;
            variable[VAR_MOMENTUM+1] *= avg;
            variable[VAR_MOMENTUM+2] *= avg;
            variable[VAR_DENSITY_ENERGY] *= avg;
        
        };
        
      auto kern = [=](cl::sycl::item<1> item) {

        //process set elements
        int n = item.get_id(0);
        if (n < set_size) {

          //user-supplied kernel call
          up_post_kernel_gpu(&arg0[n*5],
                             &arg1[n*1]);
        }

      };
      cgh.parallel_for<class up_post_kernel_kernel>(cl::sycl::range<1>(set_size), kern);
    });
    }catch(cl::sycl::exception const &e) {
    std::cout << e.what() << std::endl;exit(-1);
    }
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[18].time     += wall_t2 - wall_t1;
  OP_kernels[18].transfer += (float)set->size * arg0.size * 2.0f;
  OP_kernels[18].transfer += (float)set->size * arg1.size;
}
