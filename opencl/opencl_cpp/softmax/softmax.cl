//--------------------------------------------------
//
// kernel:  softmax
//
// Purpose: Compute the softmax out_i = exp(in_i)/sum_k(in_k)
//
// input: in
//        float vectors of length BATCH_NUM*CLASS_NUM
//
// output: out
//         float vector of length BATCH_NUM*CLASS_NUM
//

#if WORK_PER_THREAD > 2
#define CHANGE_FOR_WPT
#endif

__kernel void softmax(
   __global float* in,
   __global float* out)
{
   // each thread computes WORK_PER_THREAD outputs
   // each thread-groups has CLASS_NUM/WORK_PER_THREAD threads
   // total BATCH_NUM thread-groups
   const int l_id = get_local_id(0);
   const int g_id = get_global_id(0);
   //printf("local_id: %d\n", l_id);

   __local   float l_tmp[CLASS_NUM];
   __private volatile float p_tmp[WORK_PER_THREAD];

   // compute exp(x) and copy to local memory
   // each thread copy 2 data.
   //printf("copy data.........\n");
   volatile int g_iidx = 0;
   volatile int l_iidx = 0;
   for(int c=0; c<WORK_PER_THREAD; ++c){
     g_iidx = g_id*WORK_PER_THREAD+c;
     l_iidx = l_id*WORK_PER_THREAD+c;
     l_tmp[l_iidx] = exp(in[g_iidx]);
     p_tmp[c] = l_tmp[l_iidx];
     //printf("l_tmp[%d] = in[%d]\n", l_iidx, g_iidx);
     //printf("p_tmp[%d] = l_tmp[%d]\n", c, l_iidx);
   }

   // reduce exp(x) in local memory
   //printf("reduce data.........\n");
   volatile int hop = 1;
   volatile int l_iidx1 = 0;
   const int compute_per_group = 2*CLASS_NUM/WORK_PER_THREAD;
   //--- The reduce algorithm:
   //------- Example: CLASS_NUM = 10
   //-------          WORK_PER_THREAD = 5
   //------- thus, 2 work-item per work-group, each work-item computes 5 outputs.
   //-------
   //------- v0  v1  v2  v3  v4  v5  v6  v7  v8  v9  v10
   //-------  | + |   | + |   | + |   | + |   | + |   |
   //-------  |th0|   |th1|   |th0|   |th1|   |th0|   |
   //------- v0---+  v2---+  v4---+  v6---+  v8---+  v10
   //-------  |   +   |       |   +   |       |   +   |
   //-------  |  th0  |       |  th1  |       |  th0  |
   //------- v0-------+      v4-------+      v8-------+
   //-------  |       +       |               |
   //-------  |      th0      |               |
   //------- v0---------------+              v8
   //-------  |               +               |
   //-------  |              th0              |
   //------- v0-------------------------------+
   //-------
   do{
     l_iidx = l_id*2*hop;
#ifdef CHANGE_FOR_WPT
     do{
#endif
       l_iidx1 = l_iidx+hop;
       if(l_iidx1 < CLASS_NUM){
         l_tmp[l_iidx] = l_tmp[l_iidx]+l_tmp[l_iidx1];
         //printf("l_tmp[%d] = l_tmp[%d] + l_tmp[%d]\n", l_iidx, l_iidx, l_iidx1);
       }
#ifdef CHANGE_FOR_WPT
       l_iidx += compute_per_group*hop;
     }while( l_iidx < CLASS_NUM);
#endif
     barrier(CLK_LOCAL_MEM_FENCE);
     hop = hop * 2;
   }while(hop < CLASS_NUM);

   // calculate exp(x)/sum(exp(x))
   for(int c=0; c<WORK_PER_THREAD; ++c){
     g_iidx = g_id*WORK_PER_THREAD+c;
     out[g_iidx] = p_tmp[c]/l_tmp[0];
   }

}
