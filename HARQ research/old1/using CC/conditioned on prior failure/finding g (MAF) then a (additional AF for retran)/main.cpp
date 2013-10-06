/* Channel coding HW 4
 * Date : 2007/6/15
 * ---------------------
 * Revise
 * Date : 2007/7/25
 */

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <fstream>

#include "define.h"
#include "timer.h"

using namespace std;

#include "transmitter.h"
#include "channel.h"
#include "receiver.h"

const float start = -7;                 // starting SNR value
const float end = 5;                   // ending SNR value

const int ERROR = 200;                 // number of accumulated packet errors 
const float step = 1;                // spacing of SNR values

const int nMAF = 5000;

int diffBits(int *a, int *b, int len)
{
   int count = 0;
   for (int i=0;i<len;i++)
   {
      if (a[i] != b[i]) { count++; }
   }
   return count;   
}

int main()
{
   int points=(int)((float)(end-start)/step) + 1;
   float *snr = new float [points];
   float *snrdB = new float [points];
   float *ber = new float [points];
   float *meanMAF = new float [points];
   float *meanaAF = new float [points];
    
   // Sample SNR value
   for (int i=0;i<points;i++)
   {
      snrdB[i] = start+i*step;
      snr[i] = pow(10,snrdB[i]/10);  
   }                                                

   // Choose random seed   
   srand(time(NULL)%60);

   Timer timer;
    
   ofstream data;
   data.open("data.txt",ios::app);

   int nLoop;

   for (int i=0;i<points;i++)
   {
      cout << "Loop " << i+1 << ", SNR = " << snrdB[i] << endl;
      unsigned int bit_error = 0;  
      unsigned int packet_error=0;
      int u[k],y[k];
      float x[2*(k+m)],r[2*(k+m)],r_2[2*(k+m)],ray[2*(k+m)],null[2*(k+m)] = {0};
      float noiseBuff[2*(k+m)];
      float MAFrecord[nMAF]; 
      float aAFrecord[nMAF]; 
      
      int t;
//      for (t=0;packet_error<ERROR;t++)
      for (t=0;t<nMAF;)
      {    
         tx(u,k,x);
            
         AWGN(x,2*(k+m),snr[i],r,noiseBuff);
//         Rayleigh(x,2*(k+m),snr[i],r,ray);

         rx(r,2*(k+m),y);
         
         // If 1st decoding succeeds, continue
         if (diffBits(u,y,k) == 0) { continue; }
           
         // MAF loop
         float g = gStep;
         while (1)
         {
            // Apply MAF to x[]
            for (int l=0;l<2*(k+m);l++)
            { 
               r[l] = (1+g)*x[l] + noiseBuff[l];
            }
             
            rx(r,2*(k+m),y);
        
            // Check if decoded successfully
            if (diffBits(u,y,k) == 0) 
            {
               MAFrecord[t] = g;
               break;
            }
            
            g += gStep;
         }
         
         // Get 2nd(retran) noise
         AWGN(null,2*(k+m),snr[i],null,noiseBuff);
         
         // Additional AF for retransmission loop
         float a = 0;
         while (1)
         {
            // Apply MAF to x[]
            for (int l=0;l<2*(k+m);l++)
            { 
               r_2[l] = r[l] + a*x[l] + noiseBuff[l];
            }
             
            rx(r_2,2*(k+m),y);
        
            // Check if decoded successfully
            if (diffBits(u,y,k) == 0) 
            {
               aAFrecord[t] = a;
               break;
            }
            
            a += aStep;
         }
         
         cout << "t = " << t << endl;
         t++;
      }
      
      if (i == 0)
      {     
         data << "********************************************" << endl;
         data << "Simulation started at : "; data << timer.getStartTime(); data << endl;
      }
      
      float total = 0, total_2 = 0;
      // Clalculate mean MAF, aAF
      for (int j=0;j<nMAF;j++)
      {
         total += MAFrecord[j];
         total_2 += aAFrecord[j];
      }
      meanMAF[i] = total/(float)nMAF;
      meanaAF[i] = total_2/(float)nMAF;
      
      cout << "SNR = " << fixed << snrdB[i] << endl;
      data << "SNR = " << fixed << snrdB[i] << endl;
      cout << "mean MAF = " << meanMAF[i] << endl;
      data << "mean MAF = " << meanMAF[i] << endl;
      cout << "mean aAF = " << meanaAF[i] << endl;
      data << "mean aAF = " << meanaAF[i] << endl;
      /*
      ber[i] = bit_error/(float)(k*t);
      
      cout << "SNR = " << fixed << snrdB[i] << endl;
      data << "SNR = " << fixed << snrdB[i] << endl;
      cout << "BER = " << scientific << ber[i] << endl;
      data << "BER = " << scientific << ber[i] << endl;
       
       
      // Stopping BER : upon < 1e-5
      if (ber[i] < 1e-5)
      { 
         nLoop = i+1;
         break;
      }*/
      
      nLoop = i+1;
      cout << "end" << endl;
   }
    
  
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   // print SNR & BER
   for (int i=0;i<nLoop;i++)
   {
      data << fixed << snrdB[i] << endl;
      cout << fixed << snrdB[i] << endl;
   }
    
   data << "g-------------------------" << endl;
   cout << "g-------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << meanMAF[i] << endl;
      cout << meanMAF[i] << endl;
   }
   
   data << "a-------------------------" << endl;
   cout << "a-------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << meanaAF[i] << endl;
      cout << meanaAF[i] << endl;
   }
   
   data << "sum-----------------------" << endl;
   cout << "sum-----------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << meanMAF[i]+meanaAF[i] << endl;
      cout << meanMAF[i]+meanaAF[i] << endl;
   }
   
   data << "ratio a/g-----------------" << endl;
   cout << "ratio a/g-----------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << meanaAF[i]/meanMAF[i] << endl;
      cout << meanaAF[i]/meanMAF[i] << endl;
   }
   
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   cout << "Simulation started at  : "; timer.printStartTime(); cout << endl;
   cout << "Simulation finished at : "; timer.printCurrentTime(); cout << endl;
   cout << "Time elapsed : "; timer.printElapsedTime(); cout << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime(); data << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime(); data << endl;

   data << "********************************************" << endl;
   data.close();
    
   delete [] snr; delete [] snrdB; delete [] ber;
   
   system("pause");
   
   return 0;
}
