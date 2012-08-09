#include <cmath>
#include "modem.h"

//#define MAX_LOG 1

const float pi = 3.1415926;

inline float logSum(float a,float b)
{
   float max = (a>b) ? a : b; 
   float delta = (a>b) ? (a-b) : (b-a);
    
   if (delta>100) { delta = 100; }
   delta = log(1+exp(-delta));
    
   return max+delta;
}

//=============================PSK=======================================

PskModulator::PskModulator(int bitsPerSymbol)
{
   _m = bitsPerSymbol;
   _M = 1<<_m;
   _sqrtEs = 0;
    
   // Construct Gray table
   int size = _m; // Number of bits
   int grayTable[1<<size];
   for (int i=0;i<1<<size;i++) 
   { grayTable[i] = 0; }

   grayTable[0] = 0;
   grayTable[1] = 1;
   
   for (int i=1;i<size;i++)
   {
      for (int iRow=(1<<i);iRow<=(1<<(i+1))-1;iRow++)
      {
         for (int iCol=0;iCol<i;iCol++)
         {
             grayTable[iRow] |= grayTable[((1<<(i+1)))-iRow-1]&(1<<iCol);
         }      
         grayTable[iRow] |= (1<<i);
      }
   }

   // Construct modulation mapping table
   _modTable = new complex<float> [_M];
 
   for (int i=0;i<_M;i++)
   {
      int idx;
      for (int j=0;j<1<<_m;j++) 
      {
         if (grayTable[j]==i) { idx = j; break; }
      }

      real(_modTable[i])= cos(idx*(pi/(1<<(_m-1))));
      imag(_modTable[i])= sin(idx*(pi/(1<<(_m-1))));
   }
}

PskModulator::~PskModulator()
{
   delete _modTable;
}

complex<float> *PskModulator::bpskMod(float Es, int *v)
{
   const int symbolLen = v[0];

   complex<float> *modulatedSym = new complex<float> [symbolLen+1];
   modulatedSym[0] = symbolLen;
   
   _sqrtEs = sqrt(Es);
   
   // Mapping
   for (int i=1;i<=symbolLen;i++)
   {
      modulatedSym[i] = 2.0*_sqrtEs*v[i] - _sqrtEs;
   }
   
   return modulatedSym;
}

float *PskModulator::bpskDemod(complex<float> *r, float N0)
{
   int bitLen = (int)real(r[0]);
   float *LLR = new float [bitLen +1];
   LLR[0] = (float)bitLen;
 
   extern complex<float> *channelGain;
   
   for (int i=1;i<=bitLen;i++)
   {
      LLR[i] = 4*_sqrtEs/N0*(real(r[i])*real(channelGain[i]) + imag(r[i])*imag(channelGain[i]));
   }
   
   return LLR;
}

complex<float> *PskModulator::pskMod(float Es, int *v)
{
   const int symbolLen = v[0]/_m;
   
   complex<float> *modulatedSym = new complex<float> [symbolLen+1];
   modulatedSym[0] = symbolLen;

   _sqrtEs = sqrt(Es);

   // Mapping
   for (int i=1;i<=symbolLen;i++)
   {
      int modIdx = 0;
      for (int j=_m-1;j>=0;j--)
      {
         modIdx |= v[_m*i-j]<<j;
      }
            
      modulatedSym[i] = _sqrtEs*_modTable[modIdx];
   }
   
   return modulatedSym;
}

float *PskModulator::pskDemod(complex<float> *r, float N0)
{
   int symbolLen = (int)real(r[0]);
   int bitLen = symbolLen*_m;
   float *LLR = new float [bitLen +1];
   LLR[0] = (float)bitLen;
   
   extern complex<float> *channelGain;

   for (int i=1;i<=symbolLen;i++)
   {
      // Calculate and store in advance the likelihood of each symbol.
      float symlkhd[_M];// symbol likelihood
      for (int j=0;j<_M;j++) 
      {       
         float temp = abs( r[i] - channelGain[i]*_sqrtEs*_modTable[j] );
         // distance^2 / N0
         symlkhd[j] = -temp*temp/N0;
      }

      // For each bit within a symbol, compute its LLR.
      // Counts from the r.h.s.
      for (int j=0;j<_m;j++)
      {
         int lMask = (1<<j) - 1;
         int hMask = ~lMask;
         float lkhd1 = -1e10,lkhd0 = -1e10;
         
         // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
         for (int k=0;k<_M>>1;k++)
         {
            int lowerBits = k&lMask;
            int higherBits=(k&hMask) << 1;

            int symLkhdEntryForBit0 = higherBits | lowerBits;
            int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            
       
            float symlkhd0 = symlkhd[symLkhdEntryForBit0];
            float symlkhd1 = symlkhd[symLkhdEntryForBit1];

#ifndef MAX_LOG
            // Optimal demodulation 
            lkhd1 = logSum(lkhd1,symlkhd1);
            lkhd0 = logSum(lkhd0,symlkhd0);
#endif
#ifdef MAX_LOG
            // Sub-optimal demodulation
            if (symlkhd0>lkhd0) { lkhd0 = symlkhd0; }
            if (symlkhd1>lkhd1) { lkhd1 = symlkhd1; }
#endif
         }
    
         LLR[_m*i-j] = lkhd1 - lkhd0;
      }
   }
   
   return LLR;
}

float *PskModulator::one_pskDemod(complex<float> r, complex<float> chGain, float N0)
{
   float *LLR = new float [_m];

   // Calculate and store in advance the likelihood of each symbol.
   float symlkhd[_M];// symbol likelihood
   for (int j=0;j<_M;j++) 
   {       
      float temp = abs( r - chGain*_sqrtEs*_modTable[j] );
      // distance^2 / N0
      symlkhd[j] = -temp*temp/N0;
   }

   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<_m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      float lkhd1 = -1e10,lkhd0 = -1e10;
         
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<_M>>1;k++)
      {
         int lowerBits = k&lMask;
         int higherBits=(k&hMask) << 1;

         int symLkhdEntryForBit0 = higherBits | lowerBits;
         int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            
       
         float symlkhd0 = symlkhd[symLkhdEntryForBit0];
         float symlkhd1 = symlkhd[symLkhdEntryForBit1];

#ifndef MAX_LOG
         // Optimal demodulation 
         lkhd1 = logSum(lkhd1,symlkhd1);
         lkhd0 = logSum(lkhd0,symlkhd0);
#endif
#ifdef MAX_LOG
         // Sub-optimal demodulation
         if (symlkhd0>lkhd0) { lkhd0 = symlkhd0; }
         if (symlkhd1>lkhd1) { lkhd1 = symlkhd1; }
#endif
      }
 
      LLR[_m-j-1] = lkhd1 - lkhd0;
   }
   
   return LLR;
}

//=============================QAM=======================================

QamModulator::QamModulator(int bitsPerSymbol)
{
   _m = bitsPerSymbol;
   _M = 1<<_m;
   _sqrtE0 = 0; 

   // Construct Gray table
   int size = _m>>1; // Number of bits
   int grayTable[1<<size];
   for (int i=0;i<1<<size;i++) 
   { grayTable[i] = 0; }

   grayTable[0] = 0;
   grayTable[1] = 1;
   
   for (int i=1;i<size;i++)
   {
      for (int iRow=(1<<i);iRow<=(1<<(i+1))-1;iRow++)
      {
         for (int iCol=0;iCol<i;iCol++)
         {
             grayTable[iRow] |= grayTable[((1<<(i+1)))-iRow-1]&(1<<iCol);
         }      
         grayTable[iRow] |= (1<<i);
      }
   }

   // Construct modulation mapping table
   _modTable = new complex<float> [_M];
 
   for (int i=0;i<_M;i++)
   {
      int x = i >> (_m>>1),
          y = i & ( (1<<(_m>>1)) -1);
      
      int xIdx, yIdx;
      for (int j=0;j<1<<(_m>>1);j++) 
      {
         if (grayTable[j]==x) { xIdx = j; }
         if (grayTable[j]==y) { yIdx = j; }
      }
      
      real(_modTable[i])= ((xIdx<<1)-(1<<(_m>>1))+1);
      imag(_modTable[i])= ((yIdx<<1)-(1<<(_m>>1))+1);
   }
}

QamModulator::~QamModulator()
{
   delete _modTable;
}

complex<float> *QamModulator::qamMod(float Es, int *v)
{          
   const int symbolLen = v[0]/_m;

   complex<float> *modulatedSym = new complex<float> [symbolLen+1];
   modulatedSym[0] = symbolLen;
   
   _sqrtE0 = sqrt(3*Es/2/(_M-1));
   
   // Mapping
   for (int i=1;i<=symbolLen;i++)
   {
      int modIdx = 0;
      for (int j=_m-1;j>=0;j--)
      {
         modIdx |= v[_m*i-j]<<j;
      }
      
      modulatedSym[i] = _sqrtE0*_modTable[modIdx];
   }
   
   return modulatedSym;
}


float *QamModulator::qamDemod(complex<float> *r, float N0)
{
   int symbolLen = (int)real(r[0]);
   int bitLen = symbolLen*_m;
   float *LLR = new float [bitLen +1];
   LLR[0] = (float)bitLen;
 
   extern complex<float> *channelGain;

   for (int i=1;i<=symbolLen;i++)
   {
      // Calculate and store in advance the likelihood of each symbol.
      float symlkhd[_M];// symbol likelihood
      for (int j=0;j<_M;j++) 
      { 
         float temp = abs( r[i] - channelGain[i]*_sqrtE0*_modTable[j] );
         // distance^2 / N0
         symlkhd[j] = -temp*temp/N0;
      }

      // For each bit within a symbol, compute its LLR.
      // Counts from the r.h.s.
      for (int j=0;j<_m;j++)
      {
         int lMask = (1<<j) - 1;
         int hMask = ~lMask;
         float lkhd1 = -1e10,lkhd0 = -1e10;
         
         // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
         for (int k=0;k<_M>>1;k++)
         {
            int lowerBits = k&lMask;
            int higherBits=(k&hMask) << 1;

            int symLkhdEntryForBit0 = higherBits | lowerBits;
            int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            

            float symlkhd0 = symlkhd[symLkhdEntryForBit0];
            float symlkhd1 = symlkhd[symLkhdEntryForBit1];
            
#ifndef MAX_LOG
            // Optimal demodulation 
            lkhd1 = logSum(lkhd1,symlkhd1);
            lkhd0 = logSum(lkhd0,symlkhd0);
#endif
#ifdef MAX_LOG
            // Sub-optimal demodulation
            if (symlkhd0>lkhd0) { lkhd0 = symlkhd0; }
            if (symlkhd1>lkhd1) { lkhd1 = symlkhd1; }
#endif
         }
    
         LLR[_m*i-j] = lkhd1 - lkhd0;
      }
   }
   
   return LLR;
}

float *QamModulator::one_qamDemod(complex<float> r, complex<float> chGain, float N0)
{
   float *LLR = new float [_m];

   // Calculate and store in advance the likelihood of each symbol.
   float symlkhd[_M];// symbol likelihood
   for (int j=0;j<_M;j++) 
   { 
      float temp = abs( r - chGain*_sqrtE0*_modTable[j] );
      // distance^2 / N0
      symlkhd[j] = -temp*temp/N0;
   }

   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<_m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      float lkhd1 = -1e10,lkhd0 = -1e10;
         
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<_M>>1;k++)
      {
         int lowerBits = k&lMask;
         int higherBits=(k&hMask) << 1;

         int symLkhdEntryForBit0 = higherBits | lowerBits;
         int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            

         float symlkhd0 = symlkhd[symLkhdEntryForBit0];
         float symlkhd1 = symlkhd[symLkhdEntryForBit1];
            
#ifndef MAX_LOG
         // Optimal demodulation 
         lkhd1 = logSum(lkhd1,symlkhd1);
         lkhd0 = logSum(lkhd0,symlkhd0);
#endif
#ifdef MAX_LOG
         // Sub-optimal demodulation
         if (symlkhd0>lkhd0) { lkhd0 = symlkhd0; }
         if (symlkhd1>lkhd1) { lkhd1 = symlkhd1; }
#endif
      }
    
      LLR[_m-j-1] = lkhd1 - lkhd0;
   }
   
   return LLR;
}
