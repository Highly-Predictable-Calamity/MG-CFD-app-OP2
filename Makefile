#
# set paths for header files and libraries
#
# Some compilers/linkers are happy searching LD_LIBRARY_PATH var.
# For others (cough PGI), need to explicitly state each library
# directory.
#


ifdef OP2_INSTALL_PATH
  OP2_INC = -I$(OP2_INSTALL_PATH)/include
  OP2_LIB = -L$(OP2_INSTALL_PATH)/lib
endif

ifdef CUDA_INSTALL_PATH
  CUDA_INC = -I$(CUDA_INSTALL_PATH)/include
  CUDA_LIB = -L$(CUDA_INSTALL_PATH)/lib64
endif

ifdef HDF5_INSTALL_PATH
  HDF5_INC = -I$(HDF5_INSTALL_PATH)/include
  HDF5_LIB = -L$(HDF5_INSTALL_PATH)/lib
endif
HDF5_LIB += -lhdf5 -lz

ifdef GPI_INSTALL_PATH
  GPI_INC = -I$(GPI_INSTALL_PATH)/include
  GPI_LIB = -L$(GPI_INSTALL_PATH)/lib64
endif
GPI_INC += -DHAVE_GPI
GPI_LIB += -lGPI2


PARMETIS_VER=4
ifdef PARMETIS_INSTALL_PATH
  PARMETIS_INC = -I$(PARMETIS_INSTALL_PATH)/include
  PARMETIS_LIB = -L$(PARMETIS_INSTALL_PATH)/lib
endif
PARMETIS_INC += -DHAVE_PARMETIS
PARMETIS_LIB += -lparmetis -lmetis
ifeq ($(PARMETIS_VER),4)
  PARMETIS_INC += -DPARMETIS_VER_4
endif

ifdef KAHIP_INSTALL_PATH
  KAHIP_INC = -I$(KAHIP_INSTALL_PATH)/include
  #KAHIP_INC = -I$(KAHIP_INSTALL_PATH)/include
  KAHIP_LIB = -L$(KAHIP_INSTALL_PATH)/build/parallel/parallel_src
  #KAHIP_LIB = -L$(KAHIP_INSTALL_PATH)/lib
endif
KAHIP_INC += -DHAVE_KAHIP
KAHIP_LIB += -lparhip_interface

ifdef PTSCOTCH_INSTALL_PATH
  PTSCOTCH_INC 	= -I$(PTSCOTCH_INSTALL_PATH)/src/include
  PTSCOTCH_LIB 	= -L$(PTSCOTCH_INSTALL_PATH)/build/lib
endif
PTSCOTCH_INC += -DHAVE_PTSCOTCH
PTSCOTCH_LIB += -lptscotch -lscotch -lptscotcherr



ifdef DEBUG
  OPTIMISE := -pg -g -O0
else
  OPTIMISE := -O3 -DVECTORIZE
endif

BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src


#
# Locate MPI compilers:
#
ifdef MPI_INSTALL_PATH
  ifneq ("","$(wildcard $(MPI_INSTALL_PATH)/bin/mpicxx)")
    MPICPP := $(MPI_INSTALL_PATH)/bin/mpicxx
  else
  ifneq ("","$(wildcard $(MPI_INSTALL_PATH)/intel64/bin/mpicxx)")
    MPICPP := $(MPI_INSTALL_PATH)/intel64/bin/mpicxx
  else
    MPICPP := mpicxx
  endif
  endif

  ifneq ("","$(wildcard $(MPI_INSTALL_PATH)/bin/mpicc)")
    MPICC := $(MPI_INSTALL_PATH)/bin/mpicc
  else
  ifneq ("","$(wildcard $(MPI_INSTALL_PATH)/intel64/bin/mpicc)")
    MPICC := $(MPI_INSTALL_PATH)/intel64/bin/mpicc
  else
    MPICC := mpicc
  endif
  endif
else
  MPICPP := mpicxx
  MPICC  := mpicc
endif

ifdef OP2_COMPILER
  ifeq ($(COMPILER),)
    COMPILER=$(OP2_COMPILER)
  endif
endif

#
# Select compiler executable:
#
ifeq ($(COMPILER),gnu)
  CPP := g++
else
ifeq ($(COMPILER),clang)
  CPP := clang++
  MPICC += -cc=clang
  MPICPP += -cxx=clang++
else
ifeq ($(COMPILER),intel)
  CPP = icpc
else
ifeq ($(COMPILER),xl)
  CPP		 = xlc++
else
ifeq ($(COMPILER),pgi)
  CPP       	= pgc++
else
ifeq ($(COMPILER),cray)
  CPP           = CC
  MPICPP        = CC
else
ifeq ($(OP2_COMPILER),intel-sycl)
  CPP = dpcpp
else
  $(error unrecognised value for COMPILER: $(COMPILER))
endif
endif
endif
endif
endif
endif
endif

#
# Construct compiler arguments and flags:
#
ifeq ($(COMPILER),gnu)
  CFLAGS	= -fPIC -DUNIX -Wall -Wextra
  ## Disable C math function error checking, as prevents SIMD:
  CFLAGS += -fno-math-errno
  CPPFLAGS 	= $(CFLAGS)
  OMPFLAGS 	= -fopenmp
  MPIFLAGS 	= $(CPPFLAGS)
else
ifeq ($(COMPILER),clang)
  CFLAGS	= -fPIC -DUNIX -DVECTORIZE
  OPT_REPORT_OPTIONS := 
  OPT_REPORT_OPTIONS += -Rpass-missed=loop-vec ## Report vectorisation failures
  OPT_REPORT_OPTIONS += -Rpass="loop-(unroll|vec)" ## Report loop transformations
  # OPT_REPORT_OPTIONS += -Rpass-analysis=loop-vectorize ## Report WHY vectorize failed
  OPT_REPORT_OPTIONS += -fsave-optimization-record -gline-tables-only -gcolumn-info
  CFLAGS += $(OPT_REPORT_OPTIONS)
  CFLAGS += -fno-math-errno ## Disable C math function error checking, as prevents vectorisation
  OPTIMISE += -fno-unroll-loops ## Loop unrolling interferes with vectorisation
  OPTIMISE += -mcpu=native
  CPPFLAGS 	= $(CFLAGS)
  OMPFLAGS 	= -fopenmp
  MPIFLAGS 	= $(CPPFLAGS)
else
ifeq ($(COMPILER),intel)
  CFLAGS = -DMPICH_IGNORE_CXX_SEEK -inline-forceinline -DVECTORIZE -qopt-report=5
  CFLAGS += -restrict
  # CFLAGS += -parallel ## This flag intoduces a significant slowdown into 'vec' app
  # CFLAGS += -fno-alias ## This flag causes 'vec' app to fail validation, do not enable
  CFLAGS += -fmax-errors=1
  CPPFLAGS = $(CFLAGS)
  OMPFLAGS = -qopenmp
  OMPOFFLOAD = -qopenmp
  # NVCCFLAGS += -ccbin=$(MPICPP)
  MPIFLAGS	= $(CPPFLAGS)
  ifdef ISET
    OPTIMISE += -x$(ISET)
  else
    OPTIMISE += -xHost
  endif
else
ifeq ($(COMPILER),xl)
  CFLAGS	 = -qarch=pwr8 -qtune=pwr8 -qhot
  CPPFLAGS 	 = $(CFLAGS)
  OMPFLAGS	 = -qsmp=omp -qthreaded
  OMPOFFLOAD = -qsmp=omp -qoffload -Xptxas -v -g1
  MPIFLAGS	 = $(CPPFLAGS)
else
ifeq ($(COMPILER),pgi)
  CFLAGS  	=
  CPPFLAGS 	= $(CFLAGS)
  OMPFLAGS 	= -mp
  MPIFLAGS 	= $(CPPFLAGS)
  # NVCCFLAGS	+= -ccbin=$(MPICPP)
  # ACCFLAGS      = -acc -Minfo=acc -ta=tesla:cc35 -DOPENACC
  # ACCFLAGS      = -acc -DOPENACC -Minfo=acc
  ACCFLAGS      = -v -acc -DOPENACC -Minfo=acc
else
ifeq ($(COMPILER),cray)
  CFLAGS        = -h fp3 -h ipa5
  CPPFLAGS      = $(CFLAGS)
  OMPFLAGS      = -h omp
  MPIFLAGS      = $(CPPFLAGS)
else
ifeq ($(OP2_COMPILER),clang)
  CPP           = clang++
  CCFLAGS       = -O3 -ffast-math
  CPPFLAGS      = $(CCFLAGS)
  OMPFLAGS      = -I$(OMPTARGET_LIBS)/../include -fopenmp=libomp -Rpass-analysis
  OMPOFFLOAD    = $(OMPFLAGS) -fopenmp-targets=nvptx64-nvidia-cuda -ffp-contract=fast -Xcuda-ptxas -v 
  MPICC         = $(MPI_INSTALL_PATH)/bin/mpicc
  MPICPP        = $(MPI_INSTALL_PATH)/bin/mpicxx
  MPIFLAGS      = $(CPPFLAGS)
  NVCCFLAGS     = -ccbin=$(NVCC_HOST_COMPILER)
else
ifeq ($(OP2_COMPILER),sycl)
  CPP		= g++
  CC		= g++
  CCFLAGS       = -O3 
  CPPFLAGS      = $(CCFLAGS)
  SYCL_FLAGS    = -std=c++11 -fsycl
  NVCCFLAGS     = -ccbin=$(NVCC_HOST_COMPILER)
  MPICPP        = $(CC)
  MPIFLAGS      = $(CPPFLAGS)
else
ifeq ($(OP2_COMPILER),hipsycl)
  CPP           = syclcc-clang
  CC            = syclcc-clang
  SYCLCXX	= syclcc-clang
  CCFLAGS       = -O3 
  CPPFLAGS      = $(CCFLAGS)
  #SYCL_FLAGS    = --hipsycl-gpu-arch=sm_60  -Wdeprecated-declarations
  SYCL_FLAGS    += -DHIPSYCL
  NVCCFLAGS = -ccbin=$(NVCC_HOST_COMPILER)
  MPICPP        = $(CC)
  MPIFLAGS      = $(CPPFLAGS)
  ifdef BOOST_INSTALL_PATH
    SYCL_INCLUDES += -I$(BOOST_INSTALL_PATH)/include
    SYCL_LIBS += -L$(BOOST_INSTALL_PATH)/lib
  endif
  ifdef HIPSYCL_INSTALL_PATH
    SYCL_LIBS += -L$(HIPSYCL_INSTALL_PATH)/lib
  endif
else
ifeq ($(OP2_COMPILER),intel-sycl)
  ifdef DEBUG
    CCFLAGS  = -g -O0
  else
    CCFLAGS  = -O3
  endif
  CXX       = g++
  SYCLCXX   = dpcpp 
  CXXFLAGS  = $(CCFLAGS) 
  MPICXX    = $(MPI_INSTALL_PATH)/bin/mpicxx 
  MPIFLAGS  = $(CXXFLAGS)
  OMPFLAGS      = -I$(OMPTARGET_LIBS)/../include -fopenmp
  SYCL_LIB   = -L$(SYCL_INSTALL_PATH)/lib -lOpenCL
  NVCCFLAGS = -ccbin=$(NVCC_HOST_COMPILER)
  SYCL_FLAGS = -std=c++11 -fsycl -I$(SYCL_INSTALL_PATH)/include -I$(SYCL_INSTALL_PATH)/include #intel sycl
  #SYCL_FLAGS = -std=c++11 -fsycl #intel sycl
  SYCL_LINK_SEQ = -foffload-static-lib=$(OP2_INSTALL_PATH)/c/lib/libop2_sycl.a
else
  $(error unrecognised value for COMPILER: $(COMPILER))
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

ifdef CPP_WRAPPER
  CPP := $(CPP_WRAPPER)
endif
ifdef MPICPP_WRAPPER
  MPICPP := $(MPICPP_WRAPPER)
endif

#
# set flags for NVCC compilation and linking
#

ifndef NV_ARCH
  MESSAGE=select an NVIDA device to compile in CUDA, e.g. make NV_ARCH=KEPLER
  NV_ARCH=Kepler
endif
ifeq ($(NV_ARCH),Fermi)
  CODE_GEN_CUDA=-gencode arch=compute_20,code=sm_21
else
ifeq ($(NV_ARCH),Kepler)
  CODE_GEN_CUDA=-gencode arch=compute_35,code=sm_35
ifeq ($(OP2_COMPILER),hipsycl)
  SYCL_FLAGS += --hipsycl-gpu-arch=sm_35
endif
else
ifeq ($(NV_ARCH),Maxwell)
  CODE_GEN_CUDA=-gencode arch=compute_50,code=sm_50
else
ifeq ($(NV_ARCH),Pascal)
  CODE_GEN_CUDA=-gencode arch=compute_60,code=sm_60
ifeq ($(OP2_COMPILER),hipsycl)
  SYCL_FLAGS += --hipsycl-gpu-arch=sm_60
endif
else
ifeq ($(NV_ARCH),Volta)
  CODE_GEN_CUDA=-gencode arch=compute_70,code=sm_70
ifeq ($(OP2_COMPILER),hipsycl)
  SYCL_FLAGS += --hipsycl-gpu-arch=sm_70
endif
else
ifeq ($(NV_ARCH),Ampere)
  CODE_GEN_CUDA=-gencode arch=compute_80,code=sm_80
ifeq ($(OP2_COMPILER),hipsycl)
  SYCL_FLAGS += --hipsycl-gpu-arch=sm_80
endif
endif
endif
endif
endif
endif
endif

NVCCFLAGS =
ifdef NVCC_BIN
	NVCCFLAGS = -ccbin $(NVCC_BIN)
endif
NVCCFLAGS += $(CODE_GEN_CUDA) -m64 -Xptxas=-v -use_fast_math -O3


MGCFD_INCS := -Isrc -Isrc/Kernels

# Enable PAPI flag to enable performance counter monitoring with PAPI library:
ifdef PAPI
  MGCFD_INCS += -DPAPI
  MGCFD_LIBS := -lpapi -lpfm
endif


## Enable VERIFY_OP2_TIMING to perform timing measurements external to
## those performed by OP2 internally. Intended to verify whether OP2 timers
## are correct, particularly for MPI sync time.
#MGCFD_INCS += -DVERIFY_OP2_TIMING

## Enable DUMP_EXT_PERF_DATA to write out externally-collected
## performance data. Includes number of loop iterations counts of
## each kernel, and if VERIFY_OP2_TIMING is enabled then also
## its compute and sync times.
#MGCFD_INCS += -DDUMP_EXT_PERF_DATA

## Enable MEASURE_MEM_BW to shuffle timers and MPI calls in 
## 'unstructured_stream' to better measure memory GB/sec. 
## Useless if you don't also enable PAPI and monitor relevant DRAM event.
#MGCFD_INCS += -DMEASURE_MEM_BW



#all: seq openmp mpi mpi_vec mpi_openmp
#all: seq openmp mpi mpi_vec mpi_openmp cuda mpi_cuda sycl
# all: seq openmp mpi mpi_vec mpi_openmp cuda mpi_cuda openacc openmp4
parallel: N = $(shell nproc)
parallel:; @$(MAKE) -j$(N) -l$(N) all

## User-friendly wrappers around actual targets:
seq: $(BIN_DIR)/mgcfd_seq
sycl: $(BIN_DIR)/mgcfd_sycl
openmp: $(BIN_DIR)/mgcfd_openmp
mpi: $(BIN_DIR)/mgcfd_mpi
gpi: $(BIN_DIR)/mgcfd_mpi
vec: mpi_vec
mpi_vec: $(BIN_DIR)/mgcfd_mpi_vec
mpi_openmp: $(BIN_DIR)/mgcfd_mpi_openmp
cuda: $(BIN_DIR)/mgcfd_cuda
mpi_cuda: $(BIN_DIR)/mgcfd_mpi_cuda
openmp4: $(BIN_DIR)/mgcfd_openmp4
openacc: $(BIN_DIR)/mgcfd_openacc


OP2_MAIN_SRC = $(SRC_DIR)/euler3d_cpu_double.cpp

OP2_SEQ_OBJECTS := $(OBJ_DIR)/mgcfd_seq_main.o \
                   $(OBJ_DIR)/mgcfd_seq_kernels.o

OP2_SYCL_OBJECTS := $(OBJ_DIR)/mgcfd_sycl_main.o \
                   $(OBJ_DIR)/mgcfd_sycl_kernels.o

OP2_MPI_OBJECTS := $(OBJ_DIR)/mgcfd_mpi_main.o \
                   $(OBJ_DIR)/mgcfd_mpi_kernels.o

OP2_GPI_OBJECTS := $(OP2_MPI_OBJECTS)



KERNELS := calc_rms_kernel \
	calculate_cell_volumes \
	calculate_dt_kernel \
	compute_bnd_node_flux_kernel \
	compute_flux_edge_kernel \
	compute_step_factor_kernel \
	copy_double_kernel \
	count_bad_vals \
	count_non_zeros \
	dampen_ewt \
	down_kernel \
	down_v2_kernel_post \
	down_v2_kernel_pre \
	down_v2_kernel \
	get_min_dt_kernel \
	identify_differences \
	initialize_variables_kernel \
	residual_kernel \
	time_step_kernel \
	up_kernel \
	up_post_kernel \
	up_pre_kernel \
	zero_1d_array_kernel \
	zero_5d_array_kernel
SEQ_KERNELS := $(patsubst %, $(SRC_DIR)/../seq/%_seqkernel.cpp, $(KERNELS))
SYCL_KERNELS := $(patsubst %, $(SRC_DIR)/../sycl/%_kernel.cpp, $(KERNELS))
OMP_KERNELS := $(patsubst %, $(SRC_DIR)/../openmp/%_kernel.cpp, $(KERNELS))
CUDA_KERNELS := $(patsubst %, $(SRC_DIR)/../cuda/%_kernel.cu, $(KERNELS))
VEC_KERNELS := $(patsubst %, $(SRC_DIR)/../vec/%_veckernel.cpp, $(KERNELS))
ACC_KERNELS := $(patsubst %, $(SRC_DIR)/../openacc/%_acckernel.c, $(KERNELS))
OMP4_KERNELS := $(patsubst %, $(SRC_DIR)/../openmp4/%_omp4kernel.cpp, $(KERNELS))
OMP4_KERNEL_FUNCS := $(patsubst %, $(SRC_DIR)/../openmp4/%_omp4kernel_func.cpp, $(KERNELS))

all: 
	$(MPICC) $(CPPFLAGS) $(OPTIMISE) $(OP2_MAIN_SRC) $(MGCFD_LIBS)\
	-lm $(OP2_LIB) -lop2_gpi $(PARMETIS_LIB) $(KAHIP_LIB) $(PTSCOTCH_LIB) $(HDF5_LIB) $(GPI_LIB) $(OP2_INC) $(HDF5_INC) $(MGCFD_INCS) -Isrc/Kernels/ -I/sycl
  
clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*
clean_seq:
	rm -f $(BIN_DIR)/mgcfd_seq $(OP2_SEQ_OBJECTS)
clean_mpi:
	rm -f $(BIN_DIR)/mgcfd_mpi $(OP2_MPI_OBJECTS)
clean_mpi_vec:
	rm -f $(BIN_DIR)/mgcfd_mpi_vec $(OP2_MPI_VEC_OBJECTS)
clean_openmp:
	rm -f $(BIN_DIR)/mgcfd_openmp $(OP2_OMP_OBJECTS)
clean_mpi_openmp:
	rm -f $(BIN_DIR)/mgcfd_mpi_openmp $(OP2_MPI_OMP_OBJECTS)
clean_cuda:
	rm -f $(BIN_DIR)/mgcfd_cuda $(OP2_CUDA_OBJECTS)
clean_mpi_cuda:
	rm -f $(BIN_DIR)/mgcfd_mpi_cuda $(OP2_MPI_CUDA_OBJECTS)
clean_openacc:
	rm -f $(BIN_DIR)/mgcfd_openacc $(OP2_OPENACC_OBJECTS)
clean_openmp4:
	rm -f $(BIN_DIR)/mgcfd_openmp4 $(OP2_OMP4_OBJECTS)
