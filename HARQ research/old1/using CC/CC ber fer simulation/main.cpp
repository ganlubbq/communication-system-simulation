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

const float start = 0;                 // starting SNR value
const float end = 5;                   // ending SNR value

const int ERROR = 200;                 // number of accumulated packet errors 
const float step = 0.5;                // spacing of SNR values

int main()
{
   int points=(int)((float)(end-start)/step) + 1;
   float *snr = new float [points];
   float *snrdB = new float [points];
   float *ber = new float [points];
   float *fer = new float [points];
    
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
      cout << "Loop " << i+1 << endl;
      unsigned int bit_error = 0;  
      unsigned int packet_error=0;
      int u[k],y[k];
      float x[2*(k+m)],r[2*(k+m)],ray[2*(k+m)];
        
      int t;
      for (t=0;packet_error<ERROR;t++)
      {    
         tx(u,k,x);
            
         AWGN(x,2*(k+m),snr[i],r);
//         Rayleigh(x,2*(k+m),snr[i],r,ray);
            
         rx(r,2*(k+m),y);
        
         unsigned int temp = bit_error;
        
         int j = 0;
         while (j<k)
         {
            if (u[j]!=y[j]) { bit_error++; }
            j++;
         }
        
         if(bit_error > temp)
         {
            packet_error++;
            cout << "packet error = " << packet_error << endl;
         }
      }
     
      ber[i] = bit_error/(float)(k*t);
      fer[i] = ERROR/(float)t;
      
      if (i == 0)
      {     
         data << "********************************************" << endl;
         data << "Simulation started at  : "; data << timer.getStartTime(); data << endl;
      }
      
      cout << "SNR = " << fixed << snrdB[i] << endl;
      data << "SNR = " << fixed << snrdB[i] << endl;
      cout << "BER = " << scientific << ber[i] << endl;
      //data << "BER = " << scientific << ber[i] << endl;
      cout << "FER = " << scientific << fer[i] << endl;
      //data << "FER = " << scientific << fer[i] << endl;
      
      // Stopping BER : upon < 1e-5
      if (ber[i] < 1e-5)
      { 
         nLoop = i+1;
         break;
      }
      nLoop = i+1;
      
      cout << endl;
   }
    

   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   // print SNR & BER
   for (int i=0;i<nLoop;i++)
   {
      data << fixed << snrdB[i] << endl;
      cout << fixed << snrdB[i] << endl;
   }
    
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << scientific << ber[i] << endl;
      cout << scientific << ber[i] << endl;
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
    
   delete [] snr; delete [] snrdB; delete [] ber; delete [] fer;
   
   system("pause");
   
   return 0;
}
