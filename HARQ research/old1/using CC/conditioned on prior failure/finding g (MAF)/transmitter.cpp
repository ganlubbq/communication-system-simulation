#include "transmitter.h"

#include <cstdlib>

#include "define.h"
#include "cc.h"

void tx(int *u, int k, float *x)
{
  int i,j,temp1,temp2;
  int v[2*(k+m)];
  
  for (i=0;i<k;i++) { u[i] = rand()&1; }
  
  ccEncode(u,k,v);
  
  // Modulation
  for (i=0;i<2*(k+m);i++)
  {
    x[i] = 2.0*v[i] - 1.0;
  }
}
