#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"

#include "err_code.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <iomanip>
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
#define IN_DIM    (28*28)
#define BATCH_NUM (32)
#define CLASS_NUM (512)
//-----------------------END OF USER-DEFINED------------------------------------

// tolerance used in floating point comparisons
#define TOL   (0.001)
// layer0: fully connected layer
// layer1: softmax(cross entropy)
#define IN0   (BATCH_NUM*IN_DIM)
#define W0    (IN_DIM*CLASS_NUM)
#define B0    (CLASS_NUM)
#define IN1   (BATCH_NUM*CLASS_NUM)
#define OUT1  (BATCH_NUM*CLASS_NUM)

int main(void)
{

    //!!!TODO: Need to add a check function to check the work-item/work-group
    //         are set as valid values.

    //1. Initialize input/output vectors
    std::vector<float> h_in  (IN1);                // input vector
    std::vector<float> h_out (OUT1, 0xdeadbeef);   // output vector

    cl::Buffer d_in;                        // device memory used for the input  a vector
    cl::Buffer d_out;                        // device memory used for the output c vector

    // Fill vectors a and b with random float values
    for(std::vector<float>::iterator iter = h_in.begin(); iter != h_in.end(); ++iter)
    {
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
        std::string kernel_filename = "../softmax.cl";
        std::ifstream stream(kernel_filename);
        if (!stream.is_open()) {
            std::cout << "Cannot open file: " << kernel_filename << std::endl;
            exit(1);
        }
        std::string kernel_str = std::string(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));
        cl::Program program(context, kernel_str, false);
        std::string build_options =  "-DCLASS_NUM=" + std::to_string(CLASS_NUM);
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

        //2.3. Create the kernel functor and run with command queue
        auto softmax = cl::make_kernel<cl::Buffer, cl::Buffer>(program, "softmax");

        d_in   = cl::Buffer(context, begin(h_in), end(h_in), true);
        d_out  = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * OUT1);

        cl::Event softmax_event;
        softmax_event = softmax(
            cl::EnqueueArgs(
                queue
                ,cl::NDRange(OUT1/2)      // each thread computes 2 outputs
                ,cl::NDRange(CLASS_NUM/2)   // CLASS_NUM outputs per thread-groups
            ),
            d_in,
            d_out);
        softmax_event.wait();
        cl_ulong time_start, time_end;
        double total_time_ms;
        softmax_event.getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
        softmax_event.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
        total_time_ms = (time_end - time_start)/1000000.;
        std::cout << std::endl << std::setprecision(3) << "The opencl kernel ran in "
                  << total_time_ms << "ms." << std::endl;

        cl::copy(queue, d_out, begin(h_out), end(h_out));

        std::vector<float> output (OUT1, 0xdeadbeef);   // serial model output
        float tmp[CLASS_NUM], sum;

        clock_t serial_start, serial_end;
        serial_start = clock();

        //2.4 Run the serial model
        for(int b=0; b < BATCH_NUM; ++b){
          sum = 0;
          for(int c = 0; c < CLASS_NUM; ++c)
          {
            tmp[c] = exp(h_in[b*CLASS_NUM+c]);
            sum += tmp[c];
          }
          for(int c = 0; c < CLASS_NUM; ++c){
            output[b*CLASS_NUM+c] = tmp[c]/sum;
          }
        }
        serial_end = clock();
        double serial_total_time_ms = double(serial_end - serial_start)/double(CLOCKS_PER_SEC)*1000;
        std::cout << std::setprecision(3) << "The serial model ran in "
                  << serial_total_time_ms << "ms." << std::endl;

        //2.4 Test the results
        int correct = 0;
        float diff;
        bool match=true;
        for(int b = 0; b < BATCH_NUM; ++b)
        {
          for(int c = 0; c < CLASS_NUM; ++c){
            diff = output[b*CLASS_NUM+c] - h_out[b*CLASS_NUM+c];   // compute errors
            if(diff*diff < TOL*TOL) {    // correct if square deviation is less
              correct++;                 //  than tolerance squared
            }else {
              printf(" output: %f, h_out[%d][%d]: %f\n",
                    output[b*CLASS_NUM+c],
                    b,
                    c,
                    h_out[b*CLASS_NUM+c]);
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
            "calculate softmax:  %d out of %d results were correct.\n",
            correct,
            OUT1);
    }
    catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr
            << "ERROR: " << err.what() << "(" << err_code(err.err()) << ")"
            << std::endl;
    }
}
