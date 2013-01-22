#include "cc.h"
#include "fileIO.h"

#include <iostream>
#include <iomanip>
using namespace std;

CC::CC(void)
{
   _G11 = (int)getValue("Generator sequence 1", "config.txt","oct");
   _G12 = (int)getValue("Generator sequence 2", "config.txt","oct");
   _m = (int)getValue("Number of memeory element", "config.txt");
   _nState = (1<<_m);
   _codeRate = 1.0/2.0;
   
   // Table for the path outputs
   _nextoutput = new int [_nState][4];
   _buildTable(_nextoutput);
   
   _trellisNode = 0;
   _previousFL = 0;
   
   _dmin = (int)getValue("dmin", "config.txt");
}

CC::~CC(void)
{
   delete [] (*_nextoutput);
   
   if (_trellisNode != 0)
   {
      for (int i=0;i<_previousFL+_m+1;i++) { delete [] _trellisNode[i]; }
      delete [] _trellisNode;
   }
}

// Construct the output bits correspondin_G to input 0 and 1 for each state
void CC::_buildTable(int _nextoutput[][4])
{ 
     for (int iState=0;iState<_nState;iState++)
     {
         _nextoutput[iState][0] = 0*(_G11>>_m&1);
         _nextoutput[iState][1] = 0*(_G12>>_m&1);
         _nextoutput[iState][2] = 1*(_G11>>_m&1);
         _nextoutput[iState][3] = 1*(_G12>>_m&1);
         for (int j=0;j<_m;j++)
         {
            int jthBit = iState>>(_m-j-1)&1;
            _nextoutput[iState][0] ^= jthBit*(_G11>>(_m-j-1)&1);
            _nextoutput[iState][1] ^= jthBit*(_G12>>(_m-j-1)&1);
            _nextoutput[iState][2] ^= jthBit*(_G11>>(_m-j-1)&1);
            _nextoutput[iState][3] ^= jthBit*(_G12>>(_m-j-1)&1);
         }
     }
}

int *CC::encode(int *u)
{
   int FL = u[0];
   
   // Store the len_Gth of encoded bits
   int codedBitsLen = (FL+_m)*2;
   int *v = new int [codedBitsLen +1];
   v[0] = codedBitsLen;
   
   for (int i=1;i<=FL+_m;i++)
   {
      int dataBit;
      
      if (i <= FL) { dataBit = u[i]; }
      else { dataBit =0; }
      
      int outputBit1 = dataBit*(_G11>>_m&1);
      int outputBit2 = dataBit*(_G12>>_m&1);
        
      for (int j=1;j<=_m;j++)
      {  
         int inputBit;
         if ((i-j <= 0) || (i-j > FL)) { inputBit = 0; }
         else { inputBit = u[i-j]; }
         
         outputBit1 ^= inputBit*(_G11>>(_m-j)&1);
         outputBit2 ^= inputBit*(_G12>>(_m-j)&1);
      }
      
      v[2*i-1] = outputBit1;
      v[2*i  ] = outputBit2;
   }

   return v;
}

int *CC::viterbiDecode(float *LLR, int nList)
{
   const int len = (int)LLR[0];
   
   float **currentMetric,**nextMetric;
   currentMetric = new float *[_nState];nextMetric = new float *[_nState];
   for (int i=0;i<_nState;i++)
   { currentMetric[i] = new float [nList];nextMetric[i] = new float [nList]; }
   
   for (int i=0;i<_nState;i++)
   { for (int j=0;j<nList;j++) { currentMetric[i][j]=0;nextMetric[i][j]=0; } }
   //for (int j=1;j<=nList;j++) { currentMetric[0][nList-j] = 65536; }
   currentMetric[0][0] = 65536;
   
   int FL = (len/2 - _m);
   //-------------------------------------------------------------------------//
   // Allocate memory for the 2d array storing trellis.
   if (FL == _previousFL) {}
   else if (_trellisNode == 0)
   {
      // Allocate
      _trellisNode = new TRELLISNODE* [FL+_m+1];
      for (int i=0;i<FL+_m+1;i++) { _trellisNode[i] = new TRELLISNODE [_nState]; }
      _previousFL = FL;
   }
   else // FL has been changed
   {
      // Delete then reallocate
      for (int i=0;i<FL+_m+1;i++) { delete [] _trellisNode[i]; }
      delete [] _trellisNode;
      _trellisNode = new TRELLISNODE* [FL+_m+1];
      for (int i=0;i<FL+_m+1;i++) { _trellisNode[i] = new TRELLISNODE [_nState]; }
      _previousFL = FL;
   }
   //-------------------------------------------------------------------------//
   // Initialize trellis nodes, fuck
   
   for (int i=0;i<FL+_m+1;i++)
   {
      for (int j=0;j<_nState;j++) { _trellisNode[i][j].initListNum(nList); }    
   }
   //-------------------------------------------------------------------------//
   // Forward recursion
   for (int iBit=1;iBit<=(FL+_m);iBit++)
   {
      for(int jState=0;jState<_nState;jState++)
      {
         // Compute the next 2 state
         int nextState0 = jState>>1;
         int nextState1 = nextState0+(1<<(_m-1));
         
         // Output bits corresponding to jState      
         int b0 = _nextoutput[jState][0]; int b1 = _nextoutput[jState][1];
         int b2 = _nextoutput[jState][2]; int b3 = _nextoutput[jState][3];
           
         // Path costs
         float w0 = LLR[2*iBit -1]*b0 + LLR[2*iBit]*b1;
         float w1 = LLR[2*iBit -1]*b2 + LLR[2*iBit]*b3;

         // For input bit 1, construct the path metric
         if (nextMetric[nextState1][0] == 0)
         {
            for (int i=0;i<nList;i++)
            {
               _trellisNode[iBit][nextState1].previousNode[i] = &_trellisNode[iBit-1][jState]; 
               _trellisNode[iBit][nextState1].inputBit[i] = 1;
               _trellisNode[iBit][nextState1].outputBit[i][0] = _nextoutput[jState][2];
               _trellisNode[iBit][nextState1].outputBit[i][1] = _nextoutput[jState][3];
               nextMetric[nextState1][i] = currentMetric[jState][i] + w1;
            }
         }
         else
         {
            float orderedMetric[nList];
            int currentMetricIdx=0, nextMetricIdx=0;

            for (int i=0;i<nList;i++)
            {
               if (nextMetric[nextState1][nextMetricIdx] >= (currentMetric[jState][currentMetricIdx] + w1))
               {
                     orderedMetric[i] = nextMetric[nextState1][nextMetricIdx];
                     _trellisNode[iBit][nextState1].previousRank[i] = nextMetricIdx;
                     nextMetricIdx++;
               }
               else
               {
                  orderedMetric[i] = currentMetric[jState][currentMetricIdx] + w1;
                  _trellisNode[iBit][nextState1].previousNode[i] = &_trellisNode[iBit-1][jState]; 
                  _trellisNode[iBit][nextState1].inputBit[i] = 1;
                  _trellisNode[iBit][nextState1].outputBit[i][0] = _nextoutput[jState][2];
                  _trellisNode[iBit][nextState1].outputBit[i][1] = _nextoutput[jState][3];
                  _trellisNode[iBit][nextState1].previousRank[i] = currentMetricIdx;
                  currentMetricIdx++;
               }
            }
            for (int i=0;i<nList;i++) { nextMetric[nextState1][i] = orderedMetric[i]; }
         }
         // For input bit 0, construct the path metric
         if (nextMetric[nextState0][0] == 0)
         {
            for (int i=0;i<nList;i++)
            {
               _trellisNode[iBit][nextState0].previousNode[i] = &_trellisNode[iBit-1][jState]; 
               _trellisNode[iBit][nextState0].inputBit[i] = 0;
               _trellisNode[iBit][nextState0].outputBit[i][0] = _nextoutput[jState][0];
               _trellisNode[iBit][nextState0].outputBit[i][1] = _nextoutput[jState][1];
               nextMetric[nextState0][i] = currentMetric[jState][i] + w0;
            }
         }
         else
         {
            float orderedMetric[nList];
            int currentMetricIdx=0, nextMetricIdx=0;
            for (int i=0;i<nList;i++)
            {
               if (nextMetric[nextState0][nextMetricIdx] >= (currentMetric[jState][currentMetricIdx] + w0))
               {
                  orderedMetric[i] = nextMetric[nextState0][nextMetricIdx];
                  _trellisNode[iBit][nextState0].previousRank[i] = nextMetricIdx;
                  nextMetricIdx++;
               }
               else
               {
                  orderedMetric[i] = currentMetric[jState][currentMetricIdx] + w0;
                  _trellisNode[iBit][nextState0].previousNode[i] = &_trellisNode[iBit-1][jState]; 
                  _trellisNode[iBit][nextState0].inputBit[i] = 0;
                  _trellisNode[iBit][nextState0].outputBit[i][0] = _nextoutput[jState][0];
                  _trellisNode[iBit][nextState0].outputBit[i][1] = _nextoutput[jState][1];
                  _trellisNode[iBit][nextState0].previousRank[i] = currentMetricIdx;
                  currentMetricIdx++;
               }
            }
            for (int i=0;i<nList;i++) { nextMetric[nextState0][i] = orderedMetric[i]; }
         }

      }

      float **pass = currentMetric;
      currentMetric = nextMetric;
      nextMetric = pass;
      for (int i=0;i<_nState;i++)
      { for (int j=0;j<nList;j++) { nextMetric[i][j]=0; } }
   }
/*   
   for (int i=0;i<nList;i++)
   { cout <<  currentMetric[0][i]<< endl;}
   getchar();
*/   
   for (int i=0;i<_nState;i++)
   { delete [] currentMetric[i];delete [] nextMetric[i]; }
   delete [] currentMetric;
   delete [] nextMetric;
   
   int *decodedBits = new int [FL +1];
   decodedBits[0] = FL;
   
   int **determinedEncodedSequence = new int *[nList];
   for (int i=0;i<nList;i++) { determinedEncodedSequence[i] = new int [len]; }
   //-------------------------------------------------------------------------//
   // Trace back

   TRELLISNODE *tracePointer = &_trellisNode[(FL+_m)][0];
   int previousRank = tracePointer->previousRank[0];//determine the list num
   tracePointer = tracePointer->previousNode[0];//determine the list num
   for (int iBit=(FL+_m)-1;iBit>=1;iBit--)
   {
      int _previousRank = previousRank;
      if (iBit <= FL) { decodedBits[iBit] = tracePointer->inputBit[_previousRank]; }
      previousRank = tracePointer->previousRank[_previousRank];
      tracePointer = tracePointer->previousNode[_previousRank];
   }
//   for (int j=1;j<=FL;j++) {cout << decodedBits[j];}cout << endl;getchar();
   
   
   int numOfCodewordWithWeight_dmin = 0;
   int OriginalWeight;
   for (int i=0;i<nList;i++)
   {   
      TRELLISNODE *tracePointer = &_trellisNode[(FL+_m)][0];
      determinedEncodedSequence[i][len-1] = tracePointer->outputBit[i][1];
      determinedEncodedSequence[i][len-2] = tracePointer->outputBit[i][0];
      int previousRank = tracePointer->previousRank[i];
      tracePointer = tracePointer->previousNode[i];
      for (int iBit=(FL+_m)-1;iBit>=1;iBit--)
      {
         int _previousRank = previousRank;
         determinedEncodedSequence[i][2*iBit-1] = tracePointer->outputBit[_previousRank][1];
         determinedEncodedSequence[i][2*iBit-2] = tracePointer->outputBit[_previousRank][0];
         previousRank = tracePointer->previousRank[_previousRank];
         tracePointer = tracePointer->previousNode[_previousRank];
      }
      
      int distance = 0;
      for (int j=0;j<len;j++)
      { if (determinedEncodedSequence[i][j]==1) { distance++; } }
//      cout << "Weight for list-" <<setw(2)<< i << " is " <<setw(2)<< distance <<endl;
//      for (int j=0;j<len;j++) {cout << determinedEncodedSequence[i][j];}cout << endl;
      
      if (distance==_dmin) { numOfCodewordWithWeight_dmin++; }
      else if (distance>_dmin) { cout << "Search done!" <<endl;break; }
   }
   
   cout << "numOfCodewordWithWeight_dmin = " << numOfCodewordWithWeight_dmin << endl;
   getchar();
   
   for (int i=0;i<nList;i++) { delete [] determinedEncodedSequence[i]; }
   delete [] determinedEncodedSequence;
   
   return decodedBits;
}



