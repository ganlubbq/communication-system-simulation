#include "cc.h"
#include "fileIO.h"

const int G11 = (int)getValue("Generator sequence 1", "config.txt","oct");
const int G12 = (int)getValue("Generator sequence 2", "config.txt","oct");

const int m = (int)getValue("Number of memeory element", "config.txt");
const int nState = (1<<m);

int *ccEncode(int *u)
{
   int FL = u[0];
   
   // Store the length of encoded bits
   int codedBitsLen = (FL+m)*2;
   int *v = new int [codedBitsLen +1];
   v[0] = codedBitsLen;

   for (int i=1;i<=FL+m;i++)
   {
      int dataBit;
      if (i <= FL) { dataBit = u[i]; }
      else { dataBit =0; }
      
      int outputBit1 = dataBit*(G11>>m&1);
      int outputBit2 = dataBit*(G12>>m&1);
        
      for (int j=1;j<=m;j++)
      {  
         int inputBit;
         if ((i-j <= 0) || (i-j > FL)) { inputBit = 0; }
         else { inputBit = u[i-j]; }
         
         outputBit1 ^= inputBit*(G11>>(m-j)&1);
         outputBit2 ^= inputBit*(G12>>(m-j)&1);
      }
      
      v[2*i-1] = outputBit1;
      v[2*i  ] = outputBit2;
   }

   return v;
}

// Construct the output bits corresponding to input 0 and 1 for each state
void buildTable(int nextoutput[][4])
{ 
     for (int iState=0;iState<nState;iState++)
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

int *viterbiDecode(float *r)
{
   const int len = (int)r[0];
   // Table for the path outputs
   
   static int (*nextoutput)[4] = new int [nState][4];
   static int tableBuilt = 0;
   if (tableBuilt == 0) { buildTable(nextoutput); tableBuilt = 1; }
 
   float *cost = new float [nState];
   float *temp = new float [nState];
   for (int i=0;i<nState;i++) { cost[i] = 0; temp[i] = 0; }
   cost[0] = 65536;
     
   struct state
   {
      struct state *from;
      int in;
   };

   int FL = (len/2 - m);
      
   struct state node[FL+m+1][nState];

   extern float *fadingAmp;
   
   // Forward recursion
   for (int iBit=1;iBit<=(FL+m);iBit++)
   {
      for(int jState=0;jState<nState;jState++)
      {
         // Next 2 state
         int nextState0 = jState>>1;
         int nextState1 = nextState0+(1<<(m-1));
         
         // Output bits corresponding to jState      
         int b0 = nextoutput[jState][0]; int b1 = nextoutput[jState][1];
         int b2 = nextoutput[jState][2]; int b3 = nextoutput[jState][3];
           
         // Path costs
         float w0 = r[2*iBit -1]*b0 + r[2*iBit]*b1;
         float w1 = r[2*iBit -1]*b2 + r[2*iBit]*b3;
               
//         float w0 = r[2*iBit -1]*b0*fadingAmp[2*iBit -1] + r[2*iBit]*b1*fadingAmp[2*iBit];
//         float w1 = r[2*iBit -1]*b2*fadingAmp[2*iBit -1] + r[2*iBit]*b3*fadingAmp[2*iBit];

         if ( temp[nextState1] == 0 || (cost[jState] + w1) > temp[nextState1] )
         {
            node[iBit][nextState1].from=&node[iBit-1][jState]; 
            node[iBit][nextState1].in=1;
            temp[nextState1]=cost[jState] + w1;
         }                       
               
         if ( temp[nextState0] == 0 || (cost[jState] + w0) > temp[nextState0] )
         {
            node[iBit][nextState0].from=&node[iBit-1][jState]; 
            node[iBit][nextState0].in=0;
            temp[nextState0]=cost[jState] + w0;
         }
      }
 
      float *pass = cost;
      cost = temp;
      temp = pass;
      for(int j=0;j<nState;j++)
      {
         temp[j] = 0;
      }
   }
      
   delete [] cost;
   delete [] temp;
   
   int *y = new int [FL +1];
   y[0] = FL;
   
   // Trace back
   struct state *temp1 = &node[(FL+m)][0];
   
   for (int iBit=(FL+m);iBit>=1;iBit--)
   {
      if (iBit <= FL) { y[iBit]=temp1->in; }
      temp1=temp1->from;
   }
   
   return y;
}
