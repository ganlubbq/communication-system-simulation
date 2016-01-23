#include "cc.h"
#include "fileIO.h"

#include <iostream>
using namespace std;

CC::CC(int FL)
{
   _G11 = (int)getValue("Generator sequence 1", "config.txt","oct");
   _G12 = (int)getValue("Generator sequence 2", "config.txt","oct");

   _m = (int)getValue("Memory order", "config.txt");
      
   _nState = (1<<_m);
   _codeRate = (float)FL/(FL+_m)/2;
   
   // Table for the path outputs
   _outputBitEachState = new int [_nState][4];
   _buildTable(_outputBitEachState);
   
   _trellisNode = 0;
   _previousFL = 0;
}

CC::~CC(void)
{
   delete [] (*_outputBitEachState);
   
   if (_trellisNode != 0)
   {
      for (int i=0;i<_previousFL+1;i++) { delete [] _trellisNode[i]; }
      delete [] _trellisNode;
   }
}

inline int isNumOfBit1Odd(int value, int bitCount)
{
}

// Construct the output bits 
void CC::_buildTable(int _outputBitEachState[][4])
{ 
}

int *CC::encode(int *u)
{
}

inline void CC::_allocateMemoryForTrellis(int FL)
{
   // Memory allocated, FL unchanged.
   if (FL == _previousFL) {}
   // Memory not allocated. Initialization.
   else if (_trellisNode == 0)
   {
      // Allocate
      _trellisNode = new TrellisNode* [FL+1];
      for (int i=0;i<FL+1;i++) { _trellisNode[i] = new TrellisNode [_nState]; }
      _previousFL = FL;
      // Memorize the state number (For trace back purpose).
      for (int i=0;i<FL+1;i++)
      { 
          for (int j=0;j<_nState;j++)
          { _trellisNode[i][j].stateNum = j; }
      }
   }
   // FL has been changed. Delete the original array then reallocate.
   else
   {
      // Not supported
   }
}

extern const int N;
extern float a[];

inline float distance(float Z, int state, int inputBit, int _m)
{

   static int hasInit = 0;
   static double mean = 0;
   if (hasInit==0)
   {
      for (int i=_m+1;i<=N;i++) { mean += a[i]; }
      float p1 = 0.5;
      mean *= p1;
      hasInit = 1;
   }  

   float realMean = mean + inputBit*a[0];

   for (int j=1;j<=_m;j++)
   {
      realMean += ((state>>(_m-j))&1)*a[j];
   }
   
   return (Z-realMean)*(Z-realMean);
}

int *CC::viterbiDecode(float *LLR)
{  
   const int len = (int)LLR[0];
   
   // Smaller the better
   float *cost = new float [_nState];
   float *temp = new float [_nState];
   for (int i=0;i<_nState;i++) { cost[i] = 0; temp[i] = 0; }
   cost[0] = 0;
     
   int FL = len;
   
   _allocateMemoryForTrellis(FL);
   
   // Forward recursion
   for (int iBit=1;iBit<=(FL);iBit++)
   {
      for(int jState=0;jState<_nState;jState+=2)
      {
         // Current 2 State
         int currentStateA = jState;
         int currentStateB = jState + 1;
         // Next 2 State
         int nextState0 = jState>>1;
         int nextState1 = nextState0+(1<<(_m-1));

         // Path costs of the current state no.1
         float pathMetricA0 = distance(LLR[iBit], currentStateA, 0, _m);
         float pathMetricA1 = distance(LLR[iBit], currentStateA, 1, _m);
         // Path costs of the current state no.2
         float pathMetricB0 = distance(LLR[iBit], currentStateB, 0, _m);
         float pathMetricB1 = distance(LLR[iBit], currentStateB, 1, _m);
         
         // Compare the two paths corresponding to input bit 0
         if ( (cost[currentStateA] + pathMetricA0) < (cost[currentStateB] + pathMetricB0) )
         {
            _trellisNode[iBit][nextState0].pPreviousState = &_trellisNode[iBit-1][currentStateA];
            temp[nextState0] = cost[currentStateA] + pathMetricA0;
         }
         else
         {
            _trellisNode[iBit][nextState0].pPreviousState = &_trellisNode[iBit-1][currentStateB];
            temp[nextState0] = cost[currentStateB] + pathMetricB0;
         }
         
         // Compare the two paths corresponding to input bit 1
         if ( (cost[currentStateA] + pathMetricA1) < (cost[currentStateB] + pathMetricB1) )
         {
            _trellisNode[iBit][nextState1].pPreviousState = &_trellisNode[iBit-1][currentStateA];
            temp[nextState1] = cost[currentStateA] + pathMetricA1;
         }
         else
         {
            _trellisNode[iBit][nextState1].pPreviousState = &_trellisNode[iBit-1][currentStateB];
            temp[nextState1] = cost[currentStateB] + pathMetricB1;
         }
      }

      // Hand-off the state metrics
      float *pass = cost;
      cost = temp;
      temp = pass;
      for(int j=0;j<_nState;j++)
      {
         temp[j] = 0;
      }
   }
   
   // Find the min cost state first
   float min = cost[0];
   int minCostState = 0;
   for (int i=1;i<_nState;i++)
   {
      if (cost[i] < min)
      { min = cost[i]; minCostState = i; }
   }
   
   delete [] cost;
   delete [] temp;
   
   int *decodedBits = new int [FL +1];
   decodedBits[0] = FL;

   // Trace back   
   struct TrellisNode *temp1 = &_trellisNode[FL][minCostState];  
   for (int iBit=FL;iBit>=1;iBit--)
   {
      if (iBit <= FL)
      {
         decodedBits[iBit] = (temp1->stateNum)>>(_m-1);
      }
      temp1=temp1->pPreviousState;
   }
   
   return decodedBits;
}



