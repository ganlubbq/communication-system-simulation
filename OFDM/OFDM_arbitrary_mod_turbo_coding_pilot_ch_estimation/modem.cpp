#include <cmath>
#include "modem.h"

#include <iostream>

using namespace std;

//#define MAX_LOG 1

const double pi = 3.141592653589793;

inline double logSum(double a,double b)
{
       
   double max = (a>b) ? a : b; 
   double delta = (a>b) ? (a-b) : (b-a);
    
   if (delta>100) { delta = 100; }
   delta = log(1+exp(-delta));
    
   return max+delta;
   
//   return log(exp(a)+exp(b));
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
   _modTable = new complex<double> [_M];
 
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

complex<double> *PskModulator::bpskMod(double Es, int *v)
{
   const int symbolLen = v[0];

   complex<double> *modulatedSym = new complex<double> [symbolLen+1];
   modulatedSym[0] = symbolLen;
   
   _sqrtEs = sqrt(Es);
   
   // Mapping
   for (int i=1;i<=symbolLen;i++)
   {
      modulatedSym[i] = 2.0*_sqrtEs*v[i] - _sqrtEs;
   }
   
   return modulatedSym;
}

double *PskModulator::bpskDemod(complex<double> *r, double N0)
{
   int bitLen = (int)real(r[0]);
   double *LLR = new double [bitLen +1];
   LLR[0] = (double)bitLen;
 
   extern complex<double> *channelGain;
   
   for (int i=1;i<=bitLen;i++)
   {
      LLR[i] = 4*_sqrtEs/N0*(real(r[i])*real(channelGain[i]) + imag(r[i])*imag(channelGain[i]));
   }
   
   return LLR;
}

complex<double> *PskModulator::pskMod(double Es, int *v)
{
   const int symbolLen = v[0]/_m;
   
   complex<double> *modulatedSym = new complex<double> [symbolLen+1];
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

double *PskModulator::pskDemod(complex<double> *r, double N0)
{
   int symbolLen = (int)real(r[0]);
   int bitLen = symbolLen*_m;
   double *LLR = new double [bitLen +1];
   LLR[0] = (double)bitLen;
   
   extern complex<double> *channelGain;

   for (int i=1;i<=symbolLen;i++)
   {
      // Calculate and store in advance the likelihood of each symbol.
      double symlkhd[_M];// symbol likelihood
      for (int j=0;j<_M;j++) 
      {       
         complex<double> temp =  r[i] - channelGain[i]*_sqrtEs*_modTable[j];
         double re = real(temp);
         double im = imag(temp);
      
         symlkhd[j] = -(re*re + im*im)/N0;
      }

      // For each bit within a symbol, compute its LLR.
      // Counts from the r.h.s.
      for (int j=0;j<_m;j++)
      {
         int lMask = (1<<j) - 1;
         int hMask = ~lMask;
         double lkhd1 = -1e10,lkhd0 = -1e10;
         
         // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
         for (int k=0;k<_M>>1;k++)
         {
            int lowerBits = k&lMask;
            int higherBits=(k&hMask) << 1;

            int symLkhdEntryForBit0 = higherBits | lowerBits;
            int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            
       
            double symlkhd0 = symlkhd[symLkhdEntryForBit0];
            double symlkhd1 = symlkhd[symLkhdEntryForBit1];

#ifndef MAX_LOG
            if (k==0) { lkhd1 = symlkhd1; lkhd0 = symlkhd0; }
            else
            {
               // Optimal demodulation 
               lkhd1 = logSum(lkhd1,symlkhd1);
               lkhd0 = logSum(lkhd0,symlkhd0);
            }
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

double *PskModulator::one_pskDemod(complex<double> r, complex<double> chGain, double N0)
{
   double *LLR = new double [_m];

   // Calculate and store in advance the likelihood of each symbol.
   double symlkhd[_M];// symbol likelihood
   for (int j=0;j<_M;j++) 
   {       
      complex<double> temp =  r - chGain*_sqrtEs*_modTable[j];
      double re = real(temp);
      double im = imag(temp);
      
      symlkhd[j] = -(re*re + im*im)/N0;
   }

   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<_m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      double lkhd1 = -1e10,lkhd0 = -1e10;
      
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<_M>>1;k++)
      {
         int lowerBits = k&lMask;
         int higherBits=(k&hMask) << 1;

         int symLkhdEntryForBit0 = higherBits | lowerBits;
         int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
       
         double symlkhd0 = symlkhd[symLkhdEntryForBit0];
         double symlkhd1 = symlkhd[symLkhdEntryForBit1];

#ifndef MAX_LOG
         if (k==0) { lkhd1 = symlkhd1; lkhd0 = symlkhd0; }
         else
         {
            // Optimal demodulation 
            lkhd1 = logSum(lkhd1,symlkhd1);
            lkhd0 = logSum(lkhd0,symlkhd0);
         }
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
   _modTable = new complex<double> [_M];
 
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

complex<double> *QamModulator::qamMod(double Es, int *v)
{          
   const int symbolLen = v[0]/_m;

   complex<double> *modulatedSym = new complex<double> [symbolLen+1];
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


double *QamModulator::qamDemod(complex<double> *r, double N0)
{
   int symbolLen = (int)real(r[0]);
   int bitLen = symbolLen*_m;
   double *LLR = new double [bitLen +1];
   LLR[0] = (double)bitLen;
 
   extern complex<double> *channelGain;

   for (int i=1;i<=symbolLen;i++)
   {
      // Calculate and store in advance the likelihood of each symbol.
      double symlkhd[_M];// symbol likelihood
      for (int j=0;j<_M;j++) 
      { 
         complex<double> temp =  r[i] - channelGain[i]*_sqrtE0*_modTable[j];
         double re = real(temp);
         double im = imag(temp);
      
         symlkhd[j] = -(re*re + im*im)/N0;
      }

      // For each bit within a symbol, compute its LLR.
      // Counts from the r.h.s.
      for (int j=0;j<_m;j++)
      {
         int lMask = (1<<j) - 1;
         int hMask = ~lMask;
         double lkhd1 = -1e10,lkhd0 = -1e10;
         
         // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
         for (int k=0;k<_M>>1;k++)
         {
            int lowerBits = k&lMask;
            int higherBits=(k&hMask) << 1;

            int symLkhdEntryForBit0 = higherBits | lowerBits;
            int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            

            double symlkhd0 = symlkhd[symLkhdEntryForBit0];
            double symlkhd1 = symlkhd[symLkhdEntryForBit1];
            
#ifndef MAX_LOG
            if (k==0) { lkhd1 = symlkhd1; lkhd0 = symlkhd0; }
            else
            {
               // Optimal demodulation 
               lkhd1 = logSum(lkhd1,symlkhd1);
               lkhd0 = logSum(lkhd0,symlkhd0);
            }
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

double *QamModulator::one_qamDemod(complex<double> r, complex<double> chGain, double N0)
{
   double *LLR = new double [_m];

   // Calculate and store in advance the likelihood of each symbol.
   double symlkhd[_M];// symbol likelihood
   for (int j=0;j<_M;j++) 
   { 
      complex<double> temp =  r - chGain*_sqrtE0*_modTable[j];
      double re = real(temp);
      double im = imag(temp);
      
      symlkhd[j] = -(re*re + im*im)/N0;
   }

   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<_m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      double lkhd1 = -1e10,lkhd0 = -1e10;
         
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<_M>>1;k++)
      {
         int lowerBits = k&lMask;
         int higherBits=(k&hMask) << 1;

         int symLkhdEntryForBit0 = higherBits | lowerBits;
         int symLkhdEntryForBit1 = symLkhdEntryForBit0 + (1<<j);
            

         double symlkhd0 = symlkhd[symLkhdEntryForBit0];
         double symlkhd1 = symlkhd[symLkhdEntryForBit1];
            
#ifndef MAX_LOG
         if (k==0) { lkhd1 = symlkhd1; lkhd0 = symlkhd0; }
         else
         {
            // Optimal demodulation 
            lkhd1 = logSum(lkhd1,symlkhd1);
            lkhd0 = logSum(lkhd0,symlkhd0);
         }
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
