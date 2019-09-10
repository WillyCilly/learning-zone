#ifndef HOST_BASICS_H
#define HOST_BASICS_H

#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"

#define MAX_VENDOR_NAME_LEN 256
#define PRINT_ERROR(ERROR_NUM) printf("ERRO:%s\n", GetErrorString(ERROR_NUM));

void PrintPlatformsDevices();

void PrintDeviceInfo(cl_device_id deviceId,
                     cl_uint device_max_work_item_dimensions,
                     size_t * device_max_work_item_sizes,
                     size_t param_size);

const char *GetErrorString(cl_int error);

char * ReadKernelFile(const char *filename);

#endif // HOST_BASICS_H
