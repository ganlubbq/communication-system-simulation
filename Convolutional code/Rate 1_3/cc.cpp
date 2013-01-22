#include "cc.h"
#include "fileIO.h"

CC::CC(int FL)
{
   _G11 = (int)getValue("Generator sequence 1", "config.txt","oct");
   _G12 = (int)getValue("Generator sequence 2", "config.txt","oct");
   _G13 = (int)getValue("Generator sequence 3", "config.txt","oct");

   for (int i=0;i<31;i++)
   {
           if (((_G11>>i)&1) == 1) { _m = i; }
      else if (((_G12>>i)&1) == 1) { _m = i; }
      else if (((_G13>>i)&1) == 1) { _m = i; }
   }
   
   _nState = (1<<_m);
   _codeRate = (float)FL/(FL+_m)/3.0;
   
   // Table for the path outputs
   _outputBitEachState = new int [_nState][6];
   _buildTable(_outputBitEachState);
   
   _trellisNode = 0;
   _previousFL = 0;
}

CC::~CC(void)
{
   delete [] (*_outputBitEachState);
   
   if (_trellisNode != 0)
   {
      for (int i=0;i<_previousFL+_m+1;i++) { delete [] _trellisNode[i]; }
      delete [] _trellisNode;
   }
}

inline int isNumOfBit1Odd(int value, int bitCount)
{
   int num = 0;
   int iShift = 0;
   while (iShift <= bitCount-1)
   {
       num ^= (value>>iShift);
       iShift++;
   }
   
   return num&1;
}

// Construct the output bits 
void CC::_buildTable(int _outputBitEachState[][6])
{ 
   for (int iState=0;iState<_nState;iState++)
   {
      _outputBitEachState[iState][0] = isNumOfBit1Odd(_G11&iState,_m);
      _outputBitEachState[iState][1] = isNumOfBit1Odd(_G12&iState,_m);
      _outputBitEachState[iState][2] = isNumOfBit1Odd(_G13&iState,_m);
      _outputBitEachState[iState][3] = isNumOfBit1Odd(_G11&(iState|(1<<_m)),_m+1);
      _outputBitEachState[iState][4] = isNumOfBit1Odd(_G12&(iState|(1<<_m)),_m+1);
      _outputBitEachState[iState][5] = isNumOfBit1Odd(_G13&(iState|(1<<_m)),_m+1);
   }
}

int *CC::encode(int *u)
{
   // Allocate space for coded bits
   int FL = u[0];
   int codedBitsLen = (FL+_m)*3;
   int *v = new int [codedBitsLen +1];
   v[0] = codedBitsLen;
   
   int _shiftRegister = 0;// Should this be the class property??
   
   // Encode the data bits
   for (int i=1;i<=FL;i++)
   {
      int inputBit = u[i];
      
      v[3*i-2] = _outputBitEachState[_shiftRegister][inputBit*3  ];
      v[3*i-1] = _outputBitEachState[_shiftRegister][inputBit*3+1];
      v[3*i  ] = _outputBitEachState[_shiftRegister][inputBit*3+2];
            
      _shiftRegister |= inputBit<<_m;
      _shiftRegister >>= 1;
   }
   // Encode the tail bits
   for (int i=1;i<=_m;i++)
   {
      v[3*(i+FL)-2] = _outputBitEachState[_shiftRegister][0];
      v[3*(i+FL)-1] = _outputBitEachState[_shiftRegister][1];
      v[3*(i+FL)  ] = _outputBitEachState[_shiftRegister][2];
        
      _shiftRegister >>= 1;
   }
   
   return v;
}

inline void CC::_allocateMemoryForTrellis(int FL)
{
   // Memory allocated, FL unchanged.
   if (FL == _previousFL) {}
   // Memory not allocated. Initialization.
   else if (_trellisNode == 0)
   {
      // Allocate
      _trellisNode = new TrellisNode* [FL+_m+1];
      for (int i=0;i<FL+_m+1;i++) { _trellisNode[i] = new TrellisNode [_nState]; }
      _previousFL = FL;
      // Memorize the state number (For trace back purpose).
      for (int i=0;i<FL+_m+1;i++)
      { 
          for (int j=0;j<_nState;j++)
          { _trellisNode[i][j].stateNum = j; }
      }
   }
   // FL has been changed. Delete the original array then reallocate.
   else
   {
      for (int i=0;i<_previousFL+_m+1;i++) { delete [] _trellisNode[i]; }
      delete [] _trellisNode;
      _trellisNode = new TrellisNode* [FL+_m+1];
      for (int i=0;i<FL+_m+1;i++) { _trellisNode[i] = new TrellisNode [_nState]; }
      _previousFL = FL;
      // Memorize the state number (For trace back purpose).
      for (int i=0;i<FL+_m+1;i++)
      { 
          for (int j=0;j<_nState;j++)
          { _trellisNode[i][j].stateNum = j; }
      }
   }
}

int *CC::viterbiDecode(float *LLR)
{
   const int len = (int)LLR[0];
   
   float *cost = new float [_nState];
   float *temp = new float [_nState];
   for (int i=0;i<_nState;i++) { cost[i] = 0; temp[i] = 0; }
   cost[0] = 65536;
     
   int FL = (len/3 - _m);
   
   _allocateMemoryForTrellis(FL);
   
   // Forward recursion
   for (int iBit=1;iBit<=(FL+_m);iBit++)
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
         float pathMetricA0 = LLR[3*iBit -2]*_outputBitEachState[currentStateA][0] + LLR[3*iBit-1]*_outputBitEachState[currentStateA][1] + LLR[3*iBit]*_outputBitEachState[currentStateA][2];         
         float pathMetricA1 = LLR[3*iBit -2]*_outputBitEachState[currentStateA][3] + LLR[3*iBit-1]*_outputBitEachState[currentStateA][4] + LLR[3*iBit]*_outputBitEachState[currentStateA][5];
         // Path costs of the current state no.2
         float pathMetricB0 = LLR[3*iBit -2]*_outputBitEachState[currentStateB][0] + LLR[3*iBit-1]*_outputBitEachState[currentStateB][1] + LLR[3*iBit]*_outputBitEachState[currentStateB][2];
         float pathMetricB1 = LLR[3*iBit -2]*_outputBitEachState[currentStateB][3] + LLR[3*iBit-1]*_outputBitEachState[currentStateB][4] + LLR[3*iBit]*_outputBitEachState[currentStateB][5];
         
         // Compare the two paths corresponding to input bit 0
         if ( (cost[currentStateA] + pathMetricA0) > (cost[currentStateB] + pathMetricB0) )
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
         if ( (cost[currentStateA] + pathMetricA1) > (cost[currentStateB] + pathMetricB1) )
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
   
   delete [] cost;
   delete [] temp;
   
   int *decodedBits = new int [FL +1];
   decodedBits[0] = FL;
   
   // Trace back
   struct TrellisNode *temp1 = &_trellisNode[(FL+_m)][0];
   for (int iBit=(FL+_m);iBit>=1;iBit--)
   {
      if (iBit <= FL)
      {
         decodedBits[iBit] = (temp1->stateNum)>>(_m-1);
      }
      temp1=temp1->pPreviousState;
   }
   
   return decodedBits;
}



