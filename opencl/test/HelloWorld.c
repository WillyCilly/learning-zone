#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "CL/cl.h"
#include "host_basics.h"

#define GEMM
#define DEBUG

#if ((defined VECTOR_ADD) || (defined SIMPLEMM))
#define ARRAY_SIZE 512
#define WORK_GROUP 2
#define TILE_ARRAY_SIZE 64
#else // GEMM a[MxK] x b[KxN] = c[MxN]
#define ARRAY_SIZE_M 1024
#define ARRAY_SIZE_N 1024
#define ARRAY_SIZE_K 1024
#define WORK_GROUP 4
#define TILE_ARRAY_SIZE_M 32
#define TILE_ARRAY_SIZE_N 32
#endif

#define BUILD_OPTIONS_LEN 256
#define BUILD_LOG_LEN 16384
#define PROGRAM_ITER 5

char *GetBuildOptions()
{
  char *macro_options = NULL;
  macro_options = (char *)calloc(BUILD_OPTIONS_LEN, sizeof(char));
  if(macro_options == NULL){
    perror("Failed to allocate macro_options");
    return NULL;
  }
#ifdef VECTOR_ADD
  sprintf(macro_options, "-DVECTOR_ADD -DARRAY_SIZE=%d -DTILE_ARRAY_SIZE=%d -DWORK_GROUP=%d",\
                          ARRAY_SIZE, TILE_ARRAY_SIZE, WORK_GROUP);
#elif defined SIMPLEMM
  sprintf(macro_options, "-DSIMPLEMM -DARRAY_SIZE=%d -DTILE_ARRAY_SIZE=%d -DWORK_GROUP=%d",\
                          ARRAY_SIZE, TILE_ARRAY_SIZE, WORK_GROUP);
#else // GEMM
  sprintf(macro_options, "-DGEMM -DARRAY_SIZE_M=%d -DARRAY_SIZE_N=%d -DARRAY_SIZE_K=%d -DTILE_ARRAY_SIZE_M=%d -DTILE_ARRAY_SIZE_N=%d -DWORK_GROUP=%d",\
                          ARRAY_SIZE_M, ARRAY_SIZE_N, ARRAY_SIZE_K,TILE_ARRAY_SIZE_M, TILE_ARRAY_SIZE_N, WORK_GROUP);
#endif

  char *debug_options = NULL;
  debug_options = (char *)calloc(BUILD_OPTIONS_LEN, sizeof(char));
  if(debug_options == NULL){
    perror("Failed to allocate macro_options");
    return NULL;
  }
#ifdef DEBUG
  sprintf(debug_options, " -g -s \"HelloWorld.cl\"");
#endif

  char *build_options;
  printf("INFO:macro_options size: %lu, debug_options size: %lu\n", strlen(macro_options), strlen(debug_options));
  build_options = (char *)calloc(strlen(macro_options)+strlen(debug_options)+1, sizeof(char));
  build_options[0] = '\0';
  strcat(build_options, macro_options);
  strcat(build_options, debug_options);

  free(macro_options);
  free(debug_options);
  return build_options;
}

cl_context CreateContext(cl_device_id * deviceIdUse, cl_platform_id * platformIdUse)
{
  cl_int errNum;
  cl_uint numPlatforms;
  cl_platform_id * platformIds;
  cl_context context = NULL;

  // get numPlatforms and platformIds
  errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
  if (errNum != CL_SUCCESS || numPlatforms <= 0){
    perror("Failed to find any OpenCL platforms.");
    PRINT_ERROR(errNum);
    return NULL;
  }else{
    printf("INFO:numPlatforms: %d\n", numPlatforms);
  }
  platformIds = (cl_platform_id *)malloc(sizeof(cl_platform_id)*numPlatforms);
  errNum = clGetPlatformIDs(numPlatforms, platformIds, NULL);
  if (errNum != CL_SUCCESS || numPlatforms <= 0){
    perror("Failed to get any OpenCL platformIds.");
    PRINT_ERROR(errNum);
    return NULL;
  }else{
    printf("INFO:Using platform 0\n");
    *platformIdUse = platformIds[0];
  }

  cl_uint numDevices;
  cl_device_id * deviceIds;
  // get numDevices and deviceIds
  errNum = clGetDeviceIDs(*platformIdUse, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
  if (errNum != CL_SUCCESS || numDevices <= 0){
    perror("Failed to find any OpenCL Devices.");
    PRINT_ERROR(errNum);
    return NULL;
  }else{
    printf("INFO:numDevices: %d\n", numDevices);
  }
  deviceIds = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);
  errNum = clGetDeviceIDs(*platformIdUse, CL_DEVICE_TYPE_ALL, 1, deviceIds, NULL);
  if (errNum != CL_SUCCESS || numPlatforms <= 0){
    perror("Failed to get any OpenCL deviceIds.");
    PRINT_ERROR(errNum);
    return NULL;
  }else{
    printf("INFO:Using device 0\n");
    *deviceIdUse = deviceIds[0];
  }
  // Create context: Platform 0, device 0
  cl_context_properties contextProperties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)*platformIdUse, 0};
  context = clCreateContext(contextProperties, 1, deviceIdUse, NULL, NULL, &errNum);
  if (errNum != CL_SUCCESS){
    perror("Could not create GPU & CPU context...");
    PRINT_ERROR(errNum);
    return NULL;
  }
  free(platformIds);
  free(deviceIds);
  return context;
}

cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
  cl_int errNum;
  cl_device_id *devices;
  cl_command_queue commandQueue = NULL;
  size_t deviceBufferSize = -1;
  // First get the size of the devices buffer
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
  if (errNum != CL_SUCCESS)
  {
    perror("Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)");
    PRINT_ERROR(errNum);
    return NULL;
  }
  if (deviceBufferSize <= 0)
  {
    perror("No devices available.");
    PRINT_ERROR(errNum);
    return NULL;
  }
  // Allocate memory for the devices buffer
  // CPP: devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
  devices = (cl_device_id *) malloc(deviceBufferSize);
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
  if (errNum != CL_SUCCESS)
  {
    perror("Failed to get device IDs");
    PRINT_ERROR(errNum);
    return NULL;
  }
  // In this example, we just choose the first available device.
  // In a real program, you would likely use all available
  // devices or choose the highest performance device based on
  // OpenCL device queries.
  commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);
  if (commandQueue == NULL)
  {
    perror("Failed to create commandQueue for device 0");
    return NULL;
  }
  *device = devices[0];
  // CPP: delete [] devices;
  free(devices);
  return commandQueue;
}

cl_program CreateProgram(cl_context context, cl_device_id device, const char* kernel){
  cl_int errNum;
  cl_program program;
  size_t sizes[] = { strlen(kernel) };
  program = clCreateProgramWithSource(context, 1, &kernel, sizes, &errNum);
  if (errNum != CL_SUCCESS)
  {
    perror("Failed to create CL program from source.");
    PRINT_ERROR(errNum);
    return NULL;
  }
  // 0: NULL, build for all devices associated with program
  char *build_options;
  build_options = (char*)calloc(BUILD_OPTIONS_LEN, sizeof(char));
  build_options = GetBuildOptions();
  printf("INFO:build_options: %s", build_options);
  errNum = clBuildProgram(program, 0, NULL, build_options, NULL, NULL);
  if (errNum != CL_SUCCESS)
  {
  // Determine the reason for the error
    char buildLog[BUILD_LOG_LEN];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
    perror("Error in kernel: ");
    perror(buildLog);
    clReleaseProgram(program);
    return NULL;
  }else{
    // Determine the reason for the error
    char buildLog[BUILD_LOG_LEN];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
    printf("%s\n",buildLog);
  }
  return program;
}

bool feqn(float *a, float *b, int n){
  for (cl_int i=0; i<n; ++i){
    if(fabs(a[i]-b[i]) >= FLT_EPSILON){
      printf("result[%d]: %f, expected: %f\n", i, a[i], b[i]);
      return false;
    }
  }
  return true;
}

int main(){

  PrintPlatformsDevices();

  // Start Host Program
  cl_context context = 0;
  cl_command_queue commandQueue = 0;
  cl_program program = 0;
  cl_device_id device = 0;
  cl_kernel kernel = 0;
  cl_int errNum;
  cl_device_id deviceIdUse=0;
  cl_platform_id platformIdUse=0;
  printf("INFO:Creating Context...\n");
  context = CreateContext(&deviceIdUse, &platformIdUse);
  if(context == NULL){
    perror("Failed to create context");
    return 1;
  }

  printf("INFO:Creating CommandQueue...\n");
  fflush(stdout);
  commandQueue = CreateCommandQueue(context, &device);
  if (commandQueue == NULL)
  {
    perror("Failed to create command queue");
    return 1;
  }
  // Get the max work_item_dimensions
  cl_uint device_max_work_item_dimensions;
  errNum = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(device_max_work_item_dimensions), &device_max_work_item_dimensions, NULL);
  if(errNum != CL_SUCCESS){
    perror("Failed to get DeviceInfo: CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS");
    PRINT_ERROR(errNum);
  }else{
    printf("INFO:CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", device_max_work_item_dimensions);
  }
  size_t device_max_work_item_sizes[device_max_work_item_dimensions];
  size_t param_size = sizeof(device_max_work_item_sizes);
  PrintDeviceInfo(device, device_max_work_item_dimensions, device_max_work_item_sizes, param_size);
  printf("INFO:Creating Program...\n");
  char *knl_text = ReadKernelFile("HelloWorld.cl");
  program = CreateProgram(context, device, knl_text);
  if (program == NULL)
  {
    perror("Failed to create program");
    return 1;
  }
  // Create OpenCL kernel
  printf("INFO:Creating Kernel...\n");
  kernel = clCreateKernel(program, "hello_kernel", &errNum);
  if (errNum != CL_SUCCESS)
  {
    perror("Failed to create kernel");
    PRINT_ERROR(errNum);
    return 1;
  }

  int a_size, b_size, c_size;

#if ((defined VECTOR_ADD) || (defined SIMPLEMM))
  a_size = ARRAY_SIZE * ARRAY_SIZE;
  b_size = ARRAY_SIZE * ARRAY_SIZE;
  c_size = ARRAY_SIZE * ARRAY_SIZE;
  float *a = (float *) calloc(a_size, sizeof(float));
  float *b = (float *) calloc(b_size, sizeof(float));
  float *result          = (float *) calloc(c_size, sizeof(float));
  float *expected_result = (float *) calloc(c_size, sizeof(float));

  printf("INFO:Output array size is %d x %d\n", ARRAY_SIZE, ARRAY_SIZE);
#else
  a_size = ARRAY_SIZE_M * ARRAY_SIZE_K;
  b_size = ARRAY_SIZE_K * ARRAY_SIZE_N;
  c_size = ARRAY_SIZE_M * ARRAY_SIZE_N;
  float *a = (float *) calloc(a_size, sizeof(float));
  float *b = (float *) calloc(b_size, sizeof(float));
  float *result          = (float *) calloc(c_size, sizeof(float));
  float *expected_result = (float *) calloc(c_size, sizeof(float));

  printf("INFO:Output array size is %d x %d\n", ARRAY_SIZE_M, ARRAY_SIZE_N);
#endif

  srand(0);
#ifdef VECTOR_ADD
  for (int i = 0; i < ARRAY_SIZE; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE; ++j){
      a[i*ARRAY_SIZE+j] = (float)(rand()%64);
      b[i*ARRAY_SIZE+j] = (float)(rand()%64);
      expected_result[i*ARRAY_SIZE+j] = a[i*ARRAY_SIZE+j]+b[i*ARRAY_SIZE+j];
    }
  }
#elif defined SIMPLEMM
  for (int i = 0; i < ARRAY_SIZE; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE; ++j){
      a[i*ARRAY_SIZE+j] = (float)(rand()%64);
      b[i*ARRAY_SIZE+j] = (float)(rand()%64);
    }
  }
  for (int i = 0; i < ARRAY_SIZE; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE; ++j)
    {
      for (int k = 0; k < ARRAY_SIZE; ++k)
      {
        expected_result[i*ARRAY_SIZE+j] += a[i*ARRAY_SIZE+k]*b[k*ARRAY_SIZE+j];
      }
    }
  }
#else // GEMM
  for (int i = 0; i < ARRAY_SIZE_M; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE_K; ++j){
      a[i*ARRAY_SIZE_K+j] = (float)(rand()%64);
    }
  }
  for (int i = 0; i < ARRAY_SIZE_K; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE_N; ++j){
      b[i*ARRAY_SIZE_N+j] = (float)(rand()%64);
    }
  }
  for (int i = 0; i < ARRAY_SIZE_M; ++i)
  {
    for (int j = 0; j < ARRAY_SIZE_N; ++j)
    {
      for (int k = 0; k < ARRAY_SIZE_K; ++k)
      {
        expected_result[i*ARRAY_SIZE_N+j] += a[i*ARRAY_SIZE_K+k]*b[k*ARRAY_SIZE_N+j];
      }
    }
  }
#endif
  // Create memory objects that will be used as arguments to
  // kernel. First create host memory arrays that will be
  // used to store the arguments to the kernel
  printf("INFO:Creating MemObjects...\n");
  printf("INFO:a_size: %d, b_size: %d, c_size: %d\n", a_size, b_size, c_size);
  // return false;
  cl_mem memObjects_a = clCreateBuffer(context, CL_MEM_READ_WRITE, a_size*sizeof(float), NULL, &errNum);
  if(errNum != CL_SUCCESS)
    PRINT_ERROR(errNum);
  cl_mem memObjects_b = clCreateBuffer(context, CL_MEM_READ_WRITE, b_size*sizeof(float), NULL, &errNum);
  if(errNum != CL_SUCCESS)
    PRINT_ERROR(errNum);
  cl_mem memObjects_result = clCreateBuffer(context, CL_MEM_READ_WRITE, c_size*sizeof(float), NULL, &errNum);
  if(errNum != CL_SUCCESS)
    PRINT_ERROR(errNum);
  clEnqueueWriteBuffer(commandQueue, memObjects_a, CL_TRUE, 0, a_size*sizeof(float), a, 0, NULL, NULL);
  clEnqueueWriteBuffer(commandQueue, memObjects_b, CL_TRUE, 0, b_size*sizeof(float), b, 0, NULL, NULL);
  if (memObjects_a == NULL || memObjects_b == NULL || memObjects_result == NULL){
    perror("Failed to create memory objects");
    return 1;
  }

  // Set the kernel arguments (result, a, b)
  printf("INFO:Setting Kernel Args...\n");

  errNum  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects_a);
  errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects_b);
  errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects_result);

  if (errNum != CL_SUCCESS)
  {
    perror("Failed to set kernel arguments");
    PRINT_ERROR(errNum);
    return 1;
  }


#if ((defined VECTOR_ADD) || (defined SIMPLEMM))
  size_t localWorkSize[2] = { (ARRAY_SIZE/WORK_GROUP)/TILE_ARRAY_SIZE, (ARRAY_SIZE/WORK_GROUP)/TILE_ARRAY_SIZE };
  size_t globalWorkSize[2] = { ARRAY_SIZE/TILE_ARRAY_SIZE, ARRAY_SIZE/TILE_ARRAY_SIZE };
#else // GEMM
  size_t localWorkSize[2] = { (ARRAY_SIZE_M/WORK_GROUP)/TILE_ARRAY_SIZE_M, (ARRAY_SIZE_N/WORK_GROUP)/TILE_ARRAY_SIZE_N };
  size_t globalWorkSize[2] = { ARRAY_SIZE_M/TILE_ARRAY_SIZE_M, ARRAY_SIZE_N/TILE_ARRAY_SIZE_N };
#endif

  // Queue the kernel up for execution across the array
  printf("INFO:Queuing kernel for execution...\n");
  double runtime_diff_ms_avg = 0.;
  cl_ulong time_start = 0, time_end = 0;
  double runtime_diff_ms = 0;
  for (int iter = 0; iter < PROGRAM_ITER; ++iter){
    cl_event kernel_event;
    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernel_event);
    if (errNum != CL_SUCCESS)
    {
      perror("Failed to queue kernel for execution");
      PRINT_ERROR(errNum);
      return 1;
    }

    clWaitForEvents(1, &kernel_event);
    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    runtime_diff_ms = (time_end - time_start) / 1000000.;
    runtime_diff_ms_avg += runtime_diff_ms;
    printf("INFO:Total time taken for pass[%d]: %.6lf msec\n", iter, runtime_diff_ms);

    // Read the output buffer back to the Host
    printf("INFO:Reading buffer...\n");
    errNum = clEnqueueReadBuffer(commandQueue, memObjects_result, CL_TRUE, 0, c_size*sizeof(float), result, 0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
      perror("Failed to read result buffer");
      PRINT_ERROR(errNum);
      return 1;
    }
    clFinish(commandQueue);

    printf("INFO:Executed program successfully.\n");

    // Check the result
    printf("INFO:Checking the result...\n");
    bool check = feqn(result, expected_result, c_size);
    if(check){
      printf("INFO:Pass!\n");
    }else{
      printf("INFO:Fail!\n");
    }
  }
  printf("INFO:Average time taken: %.6lf msec\n", runtime_diff_ms_avg/PROGRAM_ITER);
  free(a);
  free(b);
  free(result);
  free(expected_result);
  clReleaseMemObject(memObjects_a);
  clReleaseMemObject(memObjects_b);
  clReleaseMemObject(memObjects_result);
  clReleaseKernel(kernel);
  clReleaseDevice(device);
  clReleaseProgram(program);
  clReleaseCommandQueue(commandQueue);
  clReleaseContext(context);

  return 0;

}
