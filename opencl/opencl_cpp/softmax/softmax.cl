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

__kernel void softmax(
   __global float* in,
   __global float* out)
{
   // each thread computes 2 outputs
   // each thread-groups has CLASS_NUM/2 threads
   // total BATCH_NUM thread-groups
   const int l_id = get_local_id(0);
   const int g_id = get_global_id(0);
   //printf("local_id: %d\n", l_id);

   __local   float l_tmp[CLASS_NUM];
   __private float p_tmp[2];

   // compute exp(x) and copy to local memory
   // each thread copy 2 data.
   //printf("copy data.........\n");
   int g_iidx = 0, l_iidx = 0;
   for(int c=0; c<2; ++c){
     g_iidx = g_id*2+c;
     l_iidx = l_id*2+c;
     l_tmp[l_iidx] = exp(in[g_iidx]);
     p_tmp[c] = l_tmp[l_iidx];
     //printf("l_tmp[%d] = in[%d]\n", l_iidx, g_iidx);
     //printf("p_tmp[%d] = l_tmp[%d]\n", c, l_iidx);
   }

   // reduce exp(x) in local memory
   //printf("reduce data.........\n");
   int hop = 1;
   do{
     l_iidx = l_id*2*hop;
     if(l_iidx + hop < CLASS_NUM){
       l_tmp[l_iidx] = l_tmp[l_iidx]+l_tmp[l_iidx+hop];
       //printf("l_tmp[%d] = l_tmp[%d] + l_tmp[%d]\n", l_iidx, l_iidx, l_iidx+hop);
     }
     barrier(CLK_LOCAL_MEM_FENCE);
     hop = hop * 2;
   }while(hop < CLASS_NUM);

   // calculate exp(x)/sum(exp(x))
   for(int c=0; c<2; ++c){
     g_iidx = g_id*2+c;
     out[g_iidx] = p_tmp[c]/l_tmp[0];
   }

}
