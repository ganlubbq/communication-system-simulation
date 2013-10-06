#include "receiver.h"

#include <cstdlib>

#include "define.h"
#include "cc.h"

void rx(float *r, int len, int *y)
{   
  int k = len/2 - m;
  
#ifdef QUANTIZATION_BIT
  
  // Hard decision
  if (QUANTIZATION_BIT == 1)
  {
    for (int i=0;i<2*(k+m);i++)
    {
      if (r[i] > 0) { r[i] = 1.0; }
      else if (r[i] < 0) { r[i] = -1.0; }
      else { r[i] = 2.0*(rand()&1) - 1.0; }
    }
  }
  
  else
  {
    int mul = (1<<(QUANTIZATION_BIT-1));
    
    for (int i=0;i<2*(k+m);i++)
    {
      r[i] *= mul;
      r[i]  = (int)r[i];
      if (r[i] > (1<<(QUANTIZATION_BIT-1))-1) { r[i] = (1<<(QUANTIZATION_BIT-1))-1; }
      else if (r[i] < -(1<<(QUANTIZATION_BIT-1))) { r[i] = -(1<<(QUANTIZATION_BIT-1)); }
      r[i] /= mul;
    }
  }
#endif

  viterbiDecode(r,2*(k+m),y);
}

