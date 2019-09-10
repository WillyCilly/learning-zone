#ifdef VECTOR_ADD
__kernel void hello_kernel(__global const float *a, __global const float *b, __global float *result)
{
  int offset_0 = get_global_id(0)*TILE_ARRAY_SIZE;
  int offset_1 = get_global_id(1)*TILE_ARRAY_SIZE;
  int row, col;
  for (int i=0; i<TILE_ARRAY_SIZE; ++i){
    for (int j=0; j<TILE_ARRAY_SIZE; ++j){
      row = offset_0+i;
      col = offset_1+j;
      result[row*ARRAY_SIZE+col] = a[row*ARRAY_SIZE+col] + b[row*ARRAY_SIZE+col];
    }
  }
}
#elif defined SIMPLEMM
__kernel void hello_kernel(__global const float *a, __global const float *b, __global float *result)
{
  // goffset: global offset
  // loffset: local offset
  // grow, gcol: global row, global col
  int myX = get_global_id(0)*TILE_ARRAY_SIZE;
  int myY = get_global_id(1)*TILE_ARRAY_SIZE;
  //printf("group_id : (%d, %d)\n", get_group_id(0), get_group_id(1));
  //printf("global_id: (%d, %d)\n", get_global_id(0), get_global_id(1));
  //printf("local_id : (%d, %d)\n", get_local_id(0), get_local_id(1));
  int grow, gcol;
  float tmp;
  for (int i=0; i<TILE_ARRAY_SIZE; ++i){
    grow  = myY + i;
    for (int j=0; j<TILE_ARRAY_SIZE; ++j){
      gcol  = myX + j;
      tmp = 0;
      for (int k=0; k<ARRAY_SIZE; ++k){
        tmp += a[grow*ARRAY_SIZE+k] * b[k*ARRAY_SIZE+gcol];
      }
      result[grow*ARRAY_SIZE+gcol] = tmp;
      //printf("result[%d]=%d\n", grow*ARRAY_SIZE+gcol, tmp);
    }
  }
}

#elif defined GEMM
__kernel void hello_kernel(__global const float *a, __global const float *b, __global float *result)
{
  // goffset: global offset
  // loffset: local offset
  // grow, gcol: global row, global col
  int myX = get_global_id(1)*TILE_ARRAY_SIZE_N;
  int myY = get_global_id(0)*TILE_ARRAY_SIZE_M;
  //printf("group_id : (%d, %d)\n", get_group_id(0), get_group_id(1));
  //printf("global_id: (%d, %d)\n", get_global_id(0), get_global_id(1));
  //printf("local_id : (%d, %d)\n", get_local_id(0), get_local_id(1));
  int grow, gcol;
  float tmp;
  for (int i=0; i<TILE_ARRAY_SIZE_M; ++i){
    grow  = myY + i;
    for (int j=0; j<TILE_ARRAY_SIZE_N; ++j){
      gcol  = myX + j;
      tmp = 0;
      for (int k=0; k<ARRAY_SIZE_K; ++k){
        tmp += a[grow*ARRAY_SIZE_K+k] * b[k*ARRAY_SIZE_N+gcol];
      }
      result[grow*ARRAY_SIZE_N+gcol] = tmp;
      //printf("result[%d]=%d\n", grow*ARRAY_SIZE_N+gcol, tmp);
    }
  }
}
#else
#endif
