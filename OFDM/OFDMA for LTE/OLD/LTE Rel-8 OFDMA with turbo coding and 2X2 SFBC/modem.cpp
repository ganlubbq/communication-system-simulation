#include <cmath>
#include "modem.h"
#include "misc.h"
#include <iostream>

using namespace std;

//#define MAX_LOG 1

//=============================PSK=======================================

void constructMapTablePSK(int bitsPerSym, complex<double> *mapTable)
{
   // Construct mapping table for QPSK
   int m = bitsPerSym;
   int M = 1<<m;
   
   // Construct Gray table
   int size = m; // Number of bits
   int grayTable[1<<size];
   for (int i=0;i<1<<size;i++) 
   { grayTable[i] = 0; }

   grayTable[0] = 0; grayTable[1] = 1;
   
   for (int i=1;i<size;i++)
   {
      for (int iRow=(1<<i);iRow<=(1<<(i+1))-1;iRow++)
      {
         for (int iCol=0;iCol<i;iCol++)
         { grayTable[iRow] |= grayTable[((1<<(i+1)))-iRow-1]&(1<<iCol); }      
         grayTable[iRow] |= (1<<i);
      }
   }
   
   for (int i=0;i<M;i++)
   {
      int idx;
      for (int j=0;j<M;j++) 
      { if (grayTable[j]==i) { idx = j; break; } }
   
      real(mapTable[i])= cos(idx*(pi/(1<<(m-1))));
      imag(mapTable[i])= sin(idx*(pi/(1<<(m-1))));
   }
}

PskModulator::PskModulator()
{
   _sqrtEs = 0;
   
   // Construct modulation mapping table
   _mapTableBPSK = new complex<double> [2];
   _mapTableQPSK = new complex<double> [4];
   _mapTable8PSK = new complex<double> [8];
   _mapTable16PSK = new complex<double> [16];
   
   constructMapTablePSK(1, _mapTableBPSK);
   constructMapTablePSK(2, _mapTableQPSK);
   constructMapTablePSK(3, _mapTable8PSK);
   constructMapTablePSK(4, _mapTable16PSK);
}

PskModulator::~PskModulator()
{
   delete [] _mapTableBPSK;   
   delete [] _mapTableQPSK;
   delete [] _mapTable8PSK;
   delete [] _mapTable16PSK;
}

inline void computeSymlkhd(double *symlkhd, complex<double> *mapTable, 
                           complex<double> r, int bitsPerSym, double sqrtE, 
                           complex<double> chGain, double N0)
{
   int M = 1 << bitsPerSym;
   
   for (int j=0;j<M;j++) 
   {       
      complex<double> temp =  r - chGain*sqrtE*mapTable[j];
      double re = real(temp); double im = imag(temp);      
      symlkhd[j] = -(re*re + im*im)/N0;

// For debug
      if (symlkhd[j] > -1e-20) { symlkhd[j] = 10; }
//      if (symlkhd[j] == 0) { symlkhd[j] = 10; }
   }
}

double *PskModulator::one_pskDemod(complex<double> r, int bitsPerSym, double Es,
                                   complex<double> chGain, double N0)
{
   // Get some mod parameters ready
   int m = bitsPerSym;
   int M = 1<<m;
   double sqrtEs;
   if (Es == _Es) { sqrtEs = _sqrtEs; }
   else { sqrtEs = sqrt(Es); }
   
   double *LLR = new double [m];

   // Calculate and store in advance the likelihood of each symbol.
   double symlkhd[M];
        if (m == 1) { computeSymlkhd(symlkhd,_mapTableBPSK,r,1,sqrtEs,chGain,N0); }
   else if (m == 2) { computeSymlkhd(symlkhd,_mapTableQPSK,r,2,sqrtEs,chGain,N0); }
   else if (m == 3) { computeSymlkhd(symlkhd,_mapTable8PSK,r,3,sqrtEs,chGain,N0); }
   else if (m == 4) { computeSymlkhd(symlkhd,_mapTable16PSK,r,4,sqrtEs,chGain,N0); }

   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      double lkhd1 = -1e10,lkhd0 = -1e10;
      
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<M>>1;k++)
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
 
      LLR[m-j-1] = lkhd1 - lkhd0;
   }
   
   return LLR;
}

//=============================QAM=======================================


void constructMapTableQAM(int bitsPerSym, complex<double> *mapTable)
{
   int m = bitsPerSym;
   int M = 1<<m;

   // Construct Gray table
   int size = m>>1; // Number of bits
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
 
   for (int i=0;i<M;i++)
   {
      int x = i >> (m>>1),
          y = i & ( (1<<(m>>1)) -1);
      
      int xIdx, yIdx;
      for (int j=0;j<1<<(m>>1);j++) 
      {
         if (grayTable[j]==x) { xIdx = j; }
         if (grayTable[j]==y) { yIdx = j; }
      }
      
      real(mapTable[i])= ((xIdx<<1)-(1<<(m>>1))+1);
      imag(mapTable[i])= ((yIdx<<1)-(1<<(m>>1))+1);
   }
}


QamModulator::QamModulator()
{
   _sqrtE0 = 0;
   
   // Construct modulation mapping table
   _mapTable4QAM = new complex<double> [4];
   _mapTable16QAM = new complex<double> [16];
   _mapTable64QAM = new complex<double> [64];
   _mapTable256QAM = new complex<double> [256];   
   
   constructMapTableQAM(2, _mapTable4QAM);
   constructMapTableQAM(4, _mapTable16QAM);
   constructMapTableQAM(6, _mapTable64QAM);
   constructMapTableQAM(8, _mapTable256QAM);
}

QamModulator::~QamModulator()
{
   delete [] _mapTable4QAM;
   delete [] _mapTable16QAM;
   delete [] _mapTable64QAM;
   delete [] _mapTable256QAM;
}

double *QamModulator::one_qamDemod(complex<double> r, int bitsPerSym, double Es, 
                                   complex<double> chGain, double N0)
{
   // Get some mod parameters ready
   int m = bitsPerSym;
   int M = 1<<m;
   double sqrtE0;
   if (Es == _Es) { sqrtE0 = _sqrtE0; }
   else { sqrtE0 = sqrt(3.0*Es/2.0/(M-1)); }
   
   double *LLR = new double [m];

   // Calculate and store in advance the likelihood of each symbol.
   double symlkhd[M];
        if (m == 2) { computeSymlkhd(symlkhd,_mapTable4QAM,r,2,sqrtE0,chGain,N0); }
   else if (m == 4) { computeSymlkhd(symlkhd,_mapTable16QAM,r,4,sqrtE0,chGain,N0); }
   else if (m == 6) { computeSymlkhd(symlkhd,_mapTable64QAM,r,6,sqrtE0,chGain,N0); }
   else if (m == 8) { computeSymlkhd(symlkhd,_mapTable256QAM,r,8,sqrtE0,chGain,N0); }
   
   // For each bit within a symbol, compute its LLR.
   // Counts from the r.h.s.
   for (int j=0;j<m;j++)
   {
      int lMask = (1<<j) - 1;
      int hMask = ~lMask;
      double lkhd1 = -1e10,lkhd0 = -1e10;
         
      // Collect the likelihood for bit 0 and 1 from the other M/2 symbols.
      for (int k=0;k<M>>1;k++)
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
    
      LLR[m-j-1] = lkhd1 - lkhd0;
   }
   
   return LLR;
}
