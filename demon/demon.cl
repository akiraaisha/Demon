__kernel void store_keys_gpu (__global unsigned char* in, __global unsigned char* out){
    unsigned int global_addr = get_global_id(0);
    in[global_addr] = out[global_addr];
}