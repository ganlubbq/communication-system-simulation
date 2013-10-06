#include "cc.h"

#include "define.h"

void ccEncode(int *u, int k, int *v)
{  
   // Divide into 2 parts for efficiency, maybe??
   
   // Deal with the original frame
   for (int i=0;i<k;i++)
   {
      int outputBit1 = u[i]*(G11>>m&1);
      int outputBit2 = u[i]*(G12>>m&1);
        
      for (int j=1;j<=m;j++)
      {  
         int inputBit;
         if (i-j < 0) { inputBit = 0; }
         else { inputBit = u[i-j]; }
         
         outputBit1 ^= inputBit*(G11>>(m-j)&1);
         outputBit2 ^= inputBit*(G12>>(m-j)&1);
      }
      
      v[2*i]     = outputBit1;
      v[(2*i)+1] = outputBit2;
   }
   
   // Cope with the appended tail bits
   for (int i=k;i<k+m;i++)
   {
      int outputBit1 = 0*(G11>>m&1);
      int outputBit2 = 0*(G12>>m&1);
        
      for (int j=1;j<=m;j++)
      {  
         int inputBit;
         if (i-j >= k) { inputBit = 0; }
         else { inputBit = u[i-j]; }
         
         outputBit1 ^= inputBit*(G11>>(m-j)&1);
         outputBit2 ^= inputBit*(G12>>(m-j)&1);
      }
      
      v[2*i]     = outputBit1;
      v[(2*i)+1] = outputBit2;
   }
}

// Construct the output bits corresponding to input 0 and 1 for each state
void buildTable(int nextoutput[][4])
{ 
     for (int iState=0;iState<st_num;iState++)
     {
         nextoutput[iState][0] = 0*(G11>>m&1);
         nextoutput[iState][1] = 0*(G12>>m&1);
         nextoutput[iState][2] = 1*(G11>>m&1);
         nextoutput[iState][3] = 1*(G12>>m&1);
         for (int j=0;j<m;j++)
         {
            int jthBit = iState>>(m-j-1)&1;
            nextoutput[iState][0] ^= jthBit*(G11>>(m-j-1)&1);
            nextoutput[iState][1] ^= jthBit*(G12>>(m-j-1)&1);
            nextoutput[iState][2] ^= jthBit*(G11>>(m-j-1)&1);
            nextoutput[iState][3] ^= jthBit*(G12>>(m-j-1)&1);
         }
     }
}

//extern float ray[2*(k+m)];

void viterbiDecode(float *r, int len, int *y)
{
   // Table for the path outputs
   static int nextoutput[st_num][4];
   static int tableBuilt = 0;
   
   if (tableBuilt == 0) { buildTable(nextoutput); tableBuilt = 1; }
 
   float *cost = new float [st_num];
   float *temp = new float [st_num];
   for (int i=0;i<st_num;i++) { cost[i] = 0; temp[i] = 0; }
   cost[0] = 65536;
     
   struct state
   {
      struct state *from;
      int in;
   };

   int k = (len/2 - m);
      
   struct state node[k+m+1][st_num];

   // Forward recursion
   for (int iBit=0;iBit<(k+m);iBit++)
   {
      for(int jState=0;jState<st_num;jState++)
      {
         // Next 2 state
         int nextState0 = jState>>1;
         int nextState1 = nextState0+(1<<(m-1));
         
         // Output bits corresponding to jState      
         int b0 = nextoutput[jState][0]; int b1 = nextoutput[jState][1];
         int b2 = nextoutput[jState][2]; int b3 = nextoutput[jState][3];
           
         // Path costs
         float w0 = r[2*iBit]*b0 + r[2*iBit+1]*b1;
         float w1 = r[2*iBit]*b2 + r[2*iBit+1]*b3;
               
//         float w0 = r[2*iBit]*b0*ray[2*iBit] + r[2*iBit+1]*b1*ray[2*iBit+1];
//         float w1 = r[2*iBit]*b2*ray[2*iBit] + r[2*iBit+1]*b3*ray[2*iBit+1];
              
         if ( temp[nextState1] == 0 || (cost[jState] + w1) > temp[nextState1] )
         {
            node[iBit+1][nextState1].from=&node[iBit][jState]; 
            node[iBit+1][nextState1].in=1;
            temp[nextState1]=cost[jState] + w1;
         }                       
               
         if ( temp[nextState0] == 0 || (cost[jState] + w0) > temp[nextState0] )
         {
            node[iBit+1][nextState0].from=&node[iBit][jState]; 
            node[iBit+1][nextState0].in=0;
            temp[nextState0]=cost[jState] + w0;
         }
      }
 
      float *pass = cost;
      cost = temp;
      temp = pass;
      for(int j=0;j<st_num;j++)
      {
           temp[j] = 0;
      }
   }
      
   delete [] cost;
   delete [] temp;
   
   // Trace back
   struct state *temp1 = &node[(k+m)][0];
   
   for (int iBit=(k+m)-1;iBit>=0;iBit--)
   {
      if (iBit < k) { y[iBit]=temp1->in; }
      temp1=temp1->from;
   }
}
