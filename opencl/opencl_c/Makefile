EXECUTABLES = kernel.o
all: $(EXECUTABLES)

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

kernel.o: kernel.c host_basics.c
	#gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -o$@ $^ -lrt -lOpenCL
	#-lrt: librt, https://docs.oracle.com/cd/E19455-01/806-0632/6j9vm89ic/index.html
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -g -std=gnu99 -o$@ $^ -lOpenCL

clean:
	rm -f $(EXECUTABLES)
