//
// auto-generated by op2.py
//

//user function
class indirect_rw_kernel_kernel;

//host stub function
void op_par_loop_indirect_rw_kernel(char const *name, op_set set,
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
  op_timing_realloc(12);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[12].name      = name;
  OP_kernels[12].count    += 1;


  int    ninds   = 2;
  int    inds[5] = {0,0,-1,1,1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: indirect_rw_kernel\n");
  }

  //get plan
  #ifdef OP_PART_SIZE_12
    int part_size = OP_PART_SIZE_12;
  #else
    int part_size = OP_part_size;
  #endif

  op_gpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    op_plan *Plan = op_plan_get_stage(name,set,part_size,nargs,args,ninds,inds,OP_STAGE_ALL);

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
        op_gpi_wait_all_cuda(nargs, args);
      }
      #ifdef OP_BLOCK_SIZE_12
      int nthread = OP_BLOCK_SIZE_12;
      #else
      int nthread = OP_block_size;
      #endif

      int nblocks = Plan->ncolblk[col];
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


          //user fun as lambda
          auto indirect_rw_kernel_gpu = [=]( 
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
            
            };
            
          auto kern = [=](cl::sycl::nd_item<1> item) {
            double arg3_l[5];
            double arg4_l[5];


            //get sizes and shift pointers and direct-mapped data

            int blockId = blkmap[item.get_group_linear_id()  + block_offset];

            int nelem    = nelems[blockId];
            int offset_b = offset[blockId];

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
                indirect_rw_kernel_gpu(&ind_arg0[map0idx*5],
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
                item.barrier(cl::sycl::access::fence_space::local_space);
              }
            }

          };
          cgh.parallel_for<class indirect_rw_kernel_kernel>(cl::sycl::nd_range<1>(nthread*nblocks,nthread), kern);
        });
        }catch(cl::sycl::exception const &e) {
        std::cout << e.what() << std::endl;exit(-1);
        }

      }
      block_offset += Plan->ncolblk[col];
    }
    OP_kernels[12].transfer  += Plan->transfer;
    OP_kernels[12].transfer2 += Plan->transfer2;
  }
  op_gpi_set_dirtybit_cuda(nargs, args);
  op2_queue->wait();
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[12].time     += wall_t2 - wall_t1;
}
