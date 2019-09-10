//------------------------------------------------------------------------------
//
// kernel:  gemm
//
// Purpose: Compute the matrix multiplication C=A*B / C=dot(A,B)
//
// input: a, b
//        float vectors of length SIZE_M*SIZE_K, SIZE_K*SIZE_N respectively
//
// output: c
//         float vector of length SIZE_M*SIZE_N holding the dot product a * b
//

// simple mm, 2d decompose
#if GEMM==1
__kernel void gemm(
   __global float* a,
   __global float* b,
   __global float* c)
{
   const int global_row = get_global_id(0);
   const int global_col = get_global_id(1);
   //printf("global_id: (%d, %d)\n", global_row, global_col);
   float acc = 0.0f;
   for(int k=0; k<SIZE_K; ++k)  {
       acc += a[k*SIZE_M+global_row] * b[global_col*SIZE_K+k];
   }
   c[global_col*SIZE_M+global_row] = acc;
}

// tiling to local memory, 2d decompose
#elif GEMM==2
__kernel void gemm2(
   __global float* a,
   __global float* b,
   __global float* c)
{
   const int local_row = get_local_id(0); // local row ID (max: TILE_S)
   const int local_col = get_local_id(1); // local col ID (max: TILE_S)
   const int global_row = TILE_S*get_group_id(0) + local_row; // global row ID (max: SIZE_M)
   const int global_col = TILE_S*get_group_id(1) + local_col; // global col ID (max: SIZE_N)

   //printf("global_id: (%d, %d)\n", global_row, global_col);

   // Copy Asub and Bsub to local memory
   __local float Asub[TILE_S][TILE_S];
   __local float Bsub[TILE_S][TILE_S];

   const int numTiles = SIZE_K/TILE_S;
   float acc = 0.0f;
   for (int t=0; t<numTiles; ++t){
     // Load 1 tile of A and B to local memory
     const int tile_row = TILE_S*t + local_row;
     const int tile_col = TILE_S*t + local_col;
     Asub[local_col][local_row] = a[tile_col*SIZE_M + global_row];
     Bsub[local_col][local_row] = b[global_col*SIZE_K + tile_row];

     // Synchronize the thread in the work_group to get local memory Asub, Bsub
     // loop over t till numTiles, then Asub and Bsub can cover the entile K line
     barrier(CLK_LOCAL_MEM_FENCE);

     // Compute partial product acc with Asub and Bsub
     // loop over t till numTiles, then acc becomes the full product of a and b
     for(int k=0; k<TILE_S; ++k)  {
         acc += Asub[k][local_row] * Bsub[local_col][k];
     }

     // Synchronize before loading the next tile to the local memory
     barrier(CLK_LOCAL_MEM_FENCE);

   }

   c[global_col*SIZE_M+global_row] = acc;
}

// copy/compute 8 elements per work-item, tiling to local memory, 2d decompose
#elif GEMM==3
__kernel void gemm3(
   __global float* a,
   __global float* b,
   __global float* c)
{
   int local_row = get_local_id(0); // local row ID (max: TILE_S)
   int local_col = get_local_id(1); // local col ID (max: TILE_S/WPT)
   int global_row = TILE_S*get_group_id(0) + local_row; // global row ID (max: SIZE_M)
   int global_col = TILE_S/WPT*get_group_id(1) + local_col; // global col ID (max: SIZE_N/WPT)

   //printf("local_id: (%d, %d)\n", local_row, local_col);
   //printf("global_id: (%d, %d)\n", global_row, global_col);

   // Copy Asub and Bsub to local memory
   __local float Asub[TILE_S][TILE_S];
   __local float Bsub[TILE_S][TILE_S];

   const int numTiles = SIZE_K/TILE_S;
   __private float acc[WPT];
   for (int w=0; w<WPT; ++w){
     acc[w] = 0.0;
   }

   for (int t=0; t<numTiles; ++t){
     // Load one tile of A and B to local memory
     // Load WPT A and B every time
     int tile_row = TILE_S*t + local_row;
     int tile_col = TILE_S*t + local_col*WPT;
     for (int w=0; w<WPT; ++w){
       Asub[local_col*WPT+w][local_row] = a[(tile_col+w)*SIZE_M + global_row];
       Bsub[local_col*WPT+w][local_row] = b[(global_col*WPT+w)*SIZE_K + tile_row];
       //printf("Thread[%03d][%03d]: Asub[%d][%d](%f) from a[%d][%d](%f)\n",
       //global_col, global_row, local_col*WPT+w, local_row, Asub[local_col*WPT+w][local_row], (tile_col*WPT+w), global_row, a[(tile_col*WPT+w)*SIZE_M + global_row]);
       //printf("Thread[%03d][%03d]: Bsub[%d][%d](%f) from b[%d][%d](%f)\n",
       //global_col, global_row, local_col*WPT+w, local_row, Bsub[local_col*WPT+w][local_row], (global_col*WPT+w), tile_row, b[(global_col*WPT+w)*SIZE_K + tile_row]);
     }

     // Synchronize the thread in the work_group to get local memory Asub, Bsub
     // loop over t till numTiles, then Asub and Bsub can cover the entile K line
     barrier(CLK_LOCAL_MEM_FENCE);

     // Compute partial product acc with Asub and Bsub
     // loop over t till numTiles, then acc becomes the full product of a and b
     for(int k=0; k<TILE_S; ++k){
       for (int w=0; w<WPT; ++w){
         acc[w] += Asub[k][local_row] * Bsub[local_col*WPT+w][k];
         //printf("Thread[%3d][%3d]:acc[%d](%f) from Asub[%d][%d](%f) and Bsub[%d][%d](%f)\n",
        //        global_col, global_row, w, acc[w], k, local_row, Asub[k][local_row], local_col*WPT+w, k, Bsub[local_col*WPT+w][k]);
       }
     }

     // Synchronize before loading the next tile to the local memory
     barrier(CLK_LOCAL_MEM_FENCE);

   }
   for(int w=0; w<WPT; ++w){
     c[(global_col*WPT+w)*SIZE_M+global_row] = acc[w];
     //printf("Thread[%3d][%3d]:c[%d][%d] from acc[%d]: %f\n",
      //      global_col, global_row, global_col*WPT+w, global_row, w, acc[w]);
   }

}

// copy/compute 8 elements per work-item
// tiling to local memory, then to private memory, 2d decompose
#elif GEMM==4
__kernel void gemm4(
   __global float* a,
   __global float* b,
   __global float* c)
{
   const int local_row = get_local_id(0); // local row ID (max: TILE_S/WPT)
   const int local_col = get_local_id(1); // local col ID (max: TILE_S/WPT)
   const int global_row = TILE_S/WPT*get_group_id(0) + local_row; // global row ID (max: SIZE_M/WPT)
   const int global_col = TILE_S/WPT*get_group_id(1) + local_col; // global col ID (max: SIZE_N/WPT)

   //printf("local_id: (%d, %d)\n", local_row, local_col);
   //printf("global_id: (%d, %d)\n", global_row, global_col);

   // Copy Asub and Bsub to local memory
   __local float Asub[TILE_S][TILE_S];
   __local float Bsub[TILE_S][TILE_S];

   const int numTiles = SIZE_K/TILE_S;
   __private float Areg;
   __private float Breg[WPT];
   __private float acc[WPT*WPT];
   for (int w=0; w<WPT*WPT; ++w){
     acc[w] = 0.0;
   }

  for (int t=0; t<numTiles; ++t){

    int tile_row = TILE_S*t + local_row*WPT;
    int tile_col = TILE_S*t + local_col*WPT;
     // Load one tile of A and B to local memory
     // Load WPT A and B every time
    for (int wcol=0; wcol<WPT; ++wcol){
      for(int wrow=0; wrow<WPT; ++wrow){
        Asub[local_col*WPT+wcol][local_row*WPT+wrow] = a[(tile_col+wcol)*SIZE_M + global_row*WPT+wrow];
        Bsub[local_col*WPT+wcol][local_row*WPT+wrow] = b[(global_col*WPT+wcol)*SIZE_K + tile_row+wrow];
      }
    }

     // Synchronize the thread in the work_group to get local memory Asub, Bsub
     // loop over t till numTiles, then Asub and Bsub can cover the entile K line
     barrier(CLK_LOCAL_MEM_FENCE);

     // Compute partial product acc with Asub and Bsub
     // loop over t till numTiles, then acc becomes the full product of a and b
     for(int k=0; k<TILE_S; ++k){
        // Cache the values of Bsub in registers
        for (int wcol=0; wcol<WPT; ++wcol) {
          Breg[wcol] = Bsub[local_col*WPT+wcol][k];
        }

        for (int wrow=0; wrow<WPT; ++wrow){
          Areg = Asub[k][local_row*WPT+wrow];
          for (int wcol=0; wcol<WPT; ++wcol){
            acc[wcol*WPT+wrow] += Areg*Breg[wcol];
          }
        }
     }

     // Synchronize before loading the next tile to the local memory
     barrier(CLK_LOCAL_MEM_FENCE);
     for(int wcol=0; wcol<WPT; ++wcol){
       for(int wrow=0; wrow<WPT; ++wrow){
         c[(global_col*WPT+wcol)*SIZE_M+global_row*WPT+wrow] = acc[wcol*WPT+wrow];
         //printf("Thread[%02d][%02d]: c[%d][%d] = acc[%d][%d]\n",
         //global_col, global_row, (global_col*WPT+wcol), global_row*WPT+wrow, wcol, wrow);
       }
     }
   }
}

#else

#endif
