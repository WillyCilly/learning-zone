#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"

#include "util.hpp" // utility library

#include "err_code.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <iostream>
#include <fstream>

// pick up device type from compiler command line or from the default type
//#define USE_DEFAULT_DEVICE
#ifdef USE_DEFAULT_DEVICE
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif
#endif
//---------------------------USER-DEFINED---------------------------------------
#define GEMM   (4)
#define SIZE_M (1024)
#define SIZE_N (1024)
#define SIZE_K (1024)
#define TILE_S (32)
#define WPT    (8)
//-----------------------END OF USER-DEFINED------------------------------------

#define TOL    (0.001)   // tolerance used in floating point comparisons
#define MATRIX_A (SIZE_M*SIZE_K)
#define MATRIX_B (SIZE_K*SIZE_N)
#define MATRIX_C (SIZE_M*SIZE_N)

int main(void)
{

    //!!!TODO: Need to add a check function to check the work-item/work-group
    //         are set as valid values.

    //1. Initialize input/output vectors

    std::vector<float> h_a (MATRIX_A);                // a vector
    std::vector<float> h_b (MATRIX_B);                // b vector
    std::vector<float> h_c (MATRIX_C, 0xdeadbeef);   // c = dot(a, b), from compute device

    cl::Buffer d_a;                        // device memory used for the input  a vector
    cl::Buffer d_b;                        // device memory used for the input  b vector
    cl::Buffer d_c;                        // device memory used for the output c vector

    // Fill vectors a and b with random float values
    for(std::vector<float>::iterator iter = h_a.begin(); iter != h_a.end(); ++iter)
    {
      //*iter = iter-h_a.begin();
      *iter = rand() / (float)RAND_MAX;
    }
    for(std::vector<float>::iterator iter = h_b.begin(); iter != h_b.end(); ++iter)
    {
      //*iter = iter-h_b.begin();
      *iter = rand() / (float)RAND_MAX;
    }

    //2. Run OpenCL C++ APIs
    try
    {
    	  // 2.1. Create a context
        //!!!TODO: Need to choose the device
#ifdef USE_DEFAULT_DEVICE
        cl::Context context(DEVICE);
#else
        // Create a platform
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        std::cout << "The platforms are: " << std::endl;
        if (all_platforms.size() == 0){
          std::cout << "No platform found." << std::endl;
          exit(1);
        }
        for(std::vector<cl::Platform>::iterator iter = all_platforms.begin(); iter != all_platforms.end(); ++iter){
          std::cout << (iter - all_platforms.begin()) << ": " <<
          (*iter).getInfo<CL_PLATFORM_NAME>() << std::endl;
        }
        cl::Platform platform = all_platforms[0];
        std::cout << "Use platform 0: " << std::endl;
        std::cout << "  Vendor   :" << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        std::cout << "  Name     :" << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << "  Version  :" << platform.getInfo<CL_PLATFORM_VERSION>() << std::endl;
        std::cout << "  Profile  :" << platform.getInfo<CL_PLATFORM_PROFILE>() << std::endl;
        std::cout << std::endl;

        // Create a device
        std::vector<cl::Device> all_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if (all_devices.size() == 0){
          std::cout << "No device found." << std::endl;
          exit(1);
        }
        for(std::vector<cl::Device>::iterator iter = all_devices.begin(); iter != all_devices.end(); ++iter){
          std::cout << (iter - all_devices.begin()) << ": " <<
          (*iter).getInfo<CL_DEVICE_NAME>() << std::endl;
        }
        cl::Device device = all_devices[0];
        uint work_item_dimension = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>();
        std::vector<size_t> work_item_size (work_item_dimension);
        device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &work_item_size);
        std::cout << "Use device 0: " << std::endl;
        std::cout << "  Name             :" << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "  Max compute units:"<< device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
        std::cout << "  Max work item    :";
        for (std::vector<size_t>::iterator iter = work_item_size.begin(); iter != work_item_size.end(); ++iter){
          std::cout << *iter << " ";
        }
        std::cout << std::endl;
        std::cout << "  Max work-group   :" << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

        //Create context from device and platform
        cl::Context context(device);
#endif

        //2.2. Load in kernel source, creating a program object for the context
        cl::Program program(context, util::loadProgram("gemm.cl"), false);
        std::string build_options =  " -DSIZE_M="+ std::to_string(SIZE_M) +
                                     " -DSIZE_N="+ std::to_string(SIZE_N) +
                                     " -DSIZE_K="+ std::to_string(SIZE_K) +
#if GEMM==2 || GEMM==3 || GEMM==4
                                     " -DTILE_S="+ std::to_string(TILE_S) +
#endif
#if GEMM==3 || GEMM==4
                                     " -DWPT="+ std::to_string(WPT) +
#endif
                                     " -DGEMM="  + std::to_string(GEMM);
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        try
        {
          program.build(devices, build_options.c_str());
        }
        catch (cl::Error& e)
        {
          if (e.err() == CL_BUILD_PROGRAM_FAILURE)
          {
            for (cl::Device dev : devices)
            {
              // Get the build log
              std::string name     = dev.getInfo<CL_DEVICE_NAME>();
              std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
              std::cerr << "Build log for " << name << ":" << std::endl
                        << buildlog << std::endl;
            }
          }
          else
          {
            throw e;
          }
        }

        // Get the command queue
        cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);

        //2.3. Create the kernel functor
#if GEMM==1
        auto gemm = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer>(program, "gemm");
#elif GEMM==2
        auto gemm = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer>(program, "gemm2");
#elif GEMM==3
        auto gemm = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer>(program, "gemm3");
#elif GEMM==4
        auto gemm = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer>(program, "gemm4");
#else
#endif

        d_a   = cl::Buffer(context, begin(h_a), end(h_a), true);
        d_b   = cl::Buffer(context, begin(h_b), end(h_b), true);
        d_c   = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * MATRIX_C);


        cl::Event gemm_event;
        gemm_event = gemm(
            cl::EnqueueArgs(
                queue
#if GEMM==3
                ,cl::NDRange(SIZE_M, SIZE_N/WPT)
#elif GEMM==4
                ,cl::NDRange(SIZE_M/WPT, SIZE_N/WPT)
#else //GEMM=1 & 2
                ,cl::NDRange(SIZE_M, SIZE_N)
#endif

#if GEMM==2
                ,cl::NDRange(TILE_S, TILE_S)
#elif GEMM==3
                ,cl::NDRange(TILE_S, TILE_S/WPT)
#elif GEMM==4
                ,cl::NDRange(TILE_S/WPT, TILE_S/WPT)
#else // GEMM=1
#endif
            ),
            d_a,
            d_b,
            d_c);
        gemm_event.wait();
        cl_ulong time_start, time_end;
        double total_time_ms;
        gemm_event.getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
        gemm_event.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
        total_time_ms = (time_end - time_start)/1000000.;
        double numFlops = (2*(long)SIZE_K*(long)SIZE_M*(long)SIZE_N)/((total_time_ms/1000.)*1000000.);
        printf("\nThe kernels ran in %lf ms at %lf MFlops \n", total_time_ms, numFlops);

        cl::copy(queue, d_c, begin(h_c), end(h_c));

        //2.4 Test the results
        int correct = 0;
        float tmp, diff;
        bool match=true;
        for(int m = 0; m < SIZE_M; ++m)
        {
          for(int n = 0; n < SIZE_N; ++n)
          {
            tmp = 0;
            for(int k= 0; k < SIZE_K; ++k)
            {
              tmp += h_a[k*SIZE_M+m] * h_b[n*SIZE_K+k];
            }
            diff = tmp - h_c[n*SIZE_M+m];               // compute errors
            if(diff*diff < TOL*TOL) {      // correct if square deviation is less
              correct++;                 //  than tolerance squared
            }
            else {
              printf(" tmp %f h_c[%d][%d] %f\n",
                    tmp,
                    n,
                    m,
                    h_c[n*SIZE_M+m]);
              match=false;
              break;
            }
          }
          if(!match){
            break;
          }
        }

        //2.5. summarize results
        printf(
            "gemm to calculate C = dot(A,B):  %d out of %d results were correct.\n",
            correct,
            MATRIX_C);
    }
    catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr
            << "ERROR: " << err.what() << "(" << err_code(err.err()) << ")"
            << std::endl;
    }
}
