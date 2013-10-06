/* This is a turbo code simulation of generalized setup
 * terminated, SDD, rate 1/3 or 1/2
 * Date : 2007/2/16
  * ---------------------------------------------
 * fix the transmitter problem
 * Date : 2007/3/9
 */

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

#define G11 035
#define G12 023

#define m 4
#define st_num (1<<m)
      
#define iteration 8
#define k 1000                       //frame length

#define start 2                      //starting SNR value
#define end 2                        //ending SNR value

#define ERROR 100                    //number of accumulated packet errors 
#define step 0.5                     //spacing of SNR values

#include"gaussian.h"
#include"declaration.h"
#include"random_interleaver.h"
#include"transmitter.h"
#include"receiver.h"
#include"channel.h"

const int nMAF = 100;

const float INIT = 1.0;
const float ACCURACY = 0.001;

int diffBits(int *a, int *b, int len)
{
   int count = 0;
   for (int i=0;i<len;i++)
   {
      if (a[i] != b[i]) { count++; }
   }
   return count;   
}

int main(){
    int i,j=0,t,temp,temp1;
    int u[k+m],y[k],bit_error,packet_error;
    float x[(k+m)*3],r[(k+m)*3],r_2[(k+m)*3];
    int points=(int)((float)(end-start)/step) + 1;
    float *snr,*snr_db,*ber;
    int s,min,hour;
    long temptime1,temptime2;
    
    snr=(float*)malloc(points*sizeof(float));
    snr_db=(float*)malloc(points*sizeof(float));
    ber=(float*)malloc(points*sizeof(float));
    srand(time(&temptime1)%60);//seed

    float *meanMAF = new float [points];
    float *meanaAF = new float [points];
    
    ofstream data;
    data.open("data.txt",ios::app);

 
    initialize();
    
    for (i=0;i<points;i++){//sample SNR value
        snr_db[i]=start+i*step;
        snr[i]=pow(10,snr_db[i]/10);  
    }                                                
    
    for (i=0;i<points;i++){
        cout <<"第"<<i<<"圈"<<endl;
//        bit_error=0;  
//        packet_error=0;
      
        float noiseBuff[3*(k+m)];
        float null[(k+m)*3]={0};
        float MAFrecord[nMAF]; 
        float aAFrecord[nMAF]; 
        
        for (t=0;t<nMAF;)
        {
            rand_permute(k,record);//determine interleaving, can be put before input()
    
            input(u,x);//generate encoded sequece
            
            AWGN(x,(k+m)*3,snr[i],r,noiseBuff);
            
            output(r,snr[i],y);//generate output under a SNR
        
            // If 1st decoding succeeds, continue
            if (diffBits(u,y,k) == 0) { continue; }
  
            // MAF loop
            float gStep;
            if (i == 0) { gStep = INIT; }
            else { gStep = meanMAF[i-1]; }
            float g = gStep;
            int count = 2;
            while (1)
            {
               // Apply MAF to x[]
               for (int l=0;l<3*(k+m);l++)
               { 
                  r[l] = (1+g)*x[l] + noiseBuff[l];
               }
               
               output(r,snr[i],y);
        
               // Check if decoded successfully
               if (diffBits(u,y,k) == 0) 
               {
                  if (gStep < ACCURACY || gStep == ACCURACY)
                  {
                     MAFrecord[t] = g;
                     break;
                  }
                  else
                  {
                     g -= gStep;
                     gStep /= 2.0;
                     count = 0;
                  }
               }
            
               if (count == 1) { gStep /= 2; }
               
               g += gStep;
               
               count++;
            }
        
            // Get 2nd(retran) noise
            AWGN(null,3*(k+m),snr[i],null,noiseBuff);
         
            // Additional AF for retransmission loop
            float aStep = INIT;
            float a = 0;
            count = 2;
            while (1)
            {
               // Apply MAF to x[]
               for (int l=0;l<3*(k+m);l++)
               { 
                  r_2[l] = r[l] + a*x[l] + noiseBuff[l];
               }
              
               output(r_2,snr[i],y);
               
               // Check if decoded successfully
               if (diffBits(u,y,k) == 0) 
               {
                  if (a == 0 || aStep < ACCURACY || aStep == ACCURACY)
                  {
                     aAFrecord[t] = a;
                     break;
                  }
                  else
                  {
                     a -= aStep;
                     aStep /= 2.0;
                     count = 0;
                  }
               }
               
               if (count == 1) { aStep /= 2; }
               
               a += aStep;
               
               count++;
            }
         
            cout <<"t = "<<t<<endl;
            t++;
        }
        
        if (i == 0)
        {     
           data << "********************************************" << endl;
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
      
        cout << "SNR = " << fixed << snr_db[i] << endl;
        data << "SNR = " << fixed << snr_db[i] << endl;
        cout << "mean MAF = " << meanMAF[i] << endl;
        data << "mean MAF = " << meanMAF[i] << endl;
        cout << "mean aAF = " << meanaAF[i] << endl;
        data << "mean aAF = " << meanaAF[i] << endl;
 
        temp1=i+1;
    }
    
    
    time(&temptime2);//calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    

    data << "--------------------------" << endl;
    cout << "--------------------------" << endl;
    
    // print SNR & BER
    for (int i=0;i<temp1;i++)
    {
       data << fixed << snr_db[i] << endl;
       cout << fixed << snr_db[i] << endl;
    }
     
    data << "g-------------------------" << endl;
    cout << "g-------------------------" << endl;
     
    for (int i=0;i<temp1;i++)
    {
       data << meanMAF[i] << endl;
       cout << meanMAF[i] << endl;
    }
    
    data << "a-------------------------" << endl;
    cout << "a-------------------------" << endl;
      
    for (int i=0;i<temp1;i++)
    {
       data << meanaAF[i] << endl;
       cout << meanaAF[i] << endl;
    }
   
    data << "sum-----------------------" << endl;
    cout << "sum-----------------------" << endl;
     
    for (int i=0;i<temp1;i++)
    {
       data << meanMAF[i]+meanaAF[i] << endl;
       cout << meanMAF[i]+meanaAF[i] << endl;
    }
    
    data << "ratio---------------------" << endl;
    cout << "ratio---------------------" << endl;
     
    for (int i=0;i<temp1;i++)
    {
       data << meanaAF[i]/meanMAF[i] << endl;
       cout << meanaAF[i]/meanMAF[i] << endl;
    }
    
    data << "--------------------------" << endl;
    cout << "--------------------------" << endl;
   
    data << "耗時 " << hour << " 小時 " << min << " 分 " << s << " 秒 " << endl;
    cout << "耗時 " << hour << " 小時 " << min << " 分 " << s << " 秒 " << endl;
    
    data << "********************************************" << endl;
    data.close();
      
    system("pause");
}
