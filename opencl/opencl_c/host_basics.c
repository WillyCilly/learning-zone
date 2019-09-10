#include "host_basics.h"

void PrintPlatformsDevices()
 {
   // get number of platforms
   cl_uint plat_count;
   clGetPlatformIDs(0, NULL, &plat_count);

   // allocate memory, get list of platforms
   cl_platform_id *platforms = (cl_platform_id *) malloc(plat_count*sizeof(cl_platform_id));
   if(platforms == NULL)
   {
     perror("Failed to allocate platform array.\n");
   }
   clGetPlatformIDs(plat_count, platforms, NULL);

   // iterate over platforms
   for (cl_uint i = 0; i < plat_count; ++i)
   {
     // get platform vendor name
     char buf[MAX_VENDOR_NAME_LEN];
     clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,sizeof(buf), buf, NULL);
     printf("platform %d: vendor '%s'\n", i, buf);

     // get number of devices in platform
     cl_uint dev_count;
     clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &dev_count);
     cl_device_id *devices = (cl_device_id *) malloc(dev_count*sizeof(cl_device_id));
     if(devices == NULL){
       perror("Failed to allocate device array.\n");
     }

     // get list of devices in platform
     clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, dev_count, devices, NULL);

     // iterate over devices
     for (cl_uint j = 0; j < dev_count; ++j)
     {
       char buf[MAX_VENDOR_NAME_LEN];
       clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(buf), buf, NULL);
       printf("  device %d: '%s'\n", j, buf);
     }
     free(devices);
   }
   free(platforms);
 }

 void PrintDeviceInfo(cl_device_id deviceId, cl_uint device_max_work_item_dimensions, size_t * device_max_work_item_sizes, size_t param_size)
 {
   cl_device_type device_type;
   cl_uint device_max_compute_units;
   cl_ulong device_global_mem_size, device_max_constant_buffer_size, device_local_mem_size;
   cl_ulong device_max_mem_alloc_size;

   // get the device type
   cl_int errNum;
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
   if(errNum != CL_SUCCESS)
   {
     perror("Failed to get DeviceInfo: CL_DEVICE_TYPE");
     PRINT_ERROR(errNum);
   }else
   {
     printf("INFO:CL_DEVICE_TYPE: ");
     switch(device_type){
       case CL_DEVICE_TYPE_CPU: printf("CPU "); break;
       case CL_DEVICE_TYPE_GPU: printf("GPU "); break;
       case CL_DEVICE_TYPE_ACCELERATOR: printf("ACCELERATOR "); break;
       case CL_DEVICE_TYPE_DEFAULT: printf("DEFAULT "); break;
       case CL_DEVICE_TYPE_ALL: printf("ALL "); break;
       default: /*should not step in*/ printf("Failed to get the CL_DEVICE_TYPE"); break;
     }
     printf("\n");
   }
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(device_max_compute_units), &device_max_compute_units, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_MAX_COMPUTE_UNITS");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_MAX_COMPUTE_UNITS: %u\n", device_max_compute_units);
   }
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, param_size, device_max_work_item_sizes, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_MAX_WORK_ITEM_SIZES");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_MAX_WORK_ITEM_SIZES: ");
     for (cl_uint i=0; i < device_max_work_item_dimensions; ++i){
       printf("%ld ", device_max_work_item_sizes[i]);
     }
     printf("\n");
   }
   size_t device_max_work_group_size;
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(device_max_work_group_size), &device_max_work_group_size, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_MAX_WORK_GROUP_SIZE");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_MAX_WORK_GROUP_SIZE: %ld\n", device_max_work_group_size);
   }
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(device_global_mem_size), &device_global_mem_size, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_GLOBAL_MEM_SIZE");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_GLOBAL_MEM_SIZE: %lx(hex) %lu(dec)\n", device_global_mem_size, device_global_mem_size);
   }
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(device_max_constant_buffer_size), &device_max_constant_buffer_size, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %lx(hex) %lu(dec)\n", device_max_constant_buffer_size, device_max_constant_buffer_size);
   }
   errNum = clGetDeviceInfo(deviceId, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(device_local_mem_size), &device_local_mem_size, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_LOCAL_MEM_SIZE");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_LOCAL_MEM_SIZE: %lx(hex) %lu(dec)\n", device_local_mem_size, device_local_mem_size);
   }
   errNum = clGetDeviceInfo(deviceId,  CL_DEVICE_MAX_MEM_ALLOC_SIZE , sizeof(device_max_mem_alloc_size), &device_max_mem_alloc_size, NULL);
   if(errNum != CL_SUCCESS){
     perror("Failed to get DeviceInfo: CL_DEVICE_MAX_MEM_ALLOC_SIZE");
     PRINT_ERROR(errNum);
   }else{
     printf("INFO:CL_DEVICE_MAX_MEM_ALLOC_SIZE: %lx(hex) %lu(dec)\n", device_max_mem_alloc_size, device_max_mem_alloc_size);
   }
 }

const char *GetErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

char * ReadKernelFile(const char *filename)
{
  FILE *f = fopen(filename, "r");
  if(f == NULL){
    perror("Failed to open the kernel file");
    exit(EXIT_FAILURE);
  }

  // figure out file size
  if(fseek(f, 0, SEEK_END) != 0){
    perror("Failed to seek the kernel file to end");
    exit(EXIT_FAILURE);
  }
  size_t size = ftell(f);

  if(fseek(f, 0, SEEK_SET) != 0){
    perror("Failed to seek the kernel file to start");
    exit(EXIT_FAILURE);
  }

  // allocate memory, slurp in entire file
  char *result = (char *)malloc((size+1)*sizeof(char));
  if(result == NULL){
    perror("Failed to allocate the kernel file contents, size is zero or error");
    exit(EXIT_FAILURE);
  }

  // check reach the feof
  if(fread(result, 1, size, f) < size){
    if(ferror(f)){
      perror("Failed to read the kernel file contents");
      exit(EXIT_FAILURE);
    }
  }

  // close, return
  if(fclose(f) != 0){
    perror("Failed to close the kernel file");
    exit(EXIT_FAILURE);
  }
  result[size] = '\0';
  printf("INFO:Read *.cl file\n");
  return result;
}
