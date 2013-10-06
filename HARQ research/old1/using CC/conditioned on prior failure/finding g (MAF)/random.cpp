#include "random.h"

#include <cstdlib>
#include <cmath>

inline float ununiformRV(void)
{
   return (float)rand()/RAND_MAX; 
}

float normalRV(float mean, float variance)
{
   float v1,v2;
   float w;
   float y1;
   static float y2;
   static int use_last = 0;
      
   if (use_last == 0)
   {
      do
      {
         v1 = 2.0*ununiformRV() - 1.0;
         v2 = 2.0*ununiformRV() - 1.0;
         w = v1*v1 + v2*v2;
      }while (w >= 1.0);
         
      w = sqrt((-2.0*log(w))/w);
         
      y1 = v1*w;
      y2 = v2*w;
         
      use_last = 1;
   }
      
   else
   {
      y1 = y2;
           
      use_last = 0;
   }

   return (y1*sqrt(variance) + mean);
}

float rayleighRV(float meanPower)
{
   float u;

   u = sqrt(pow(normalRV(0,meanPower/2.0),2) + pow(normalRV(0,meanPower/2.0),2));

   return u;
}
