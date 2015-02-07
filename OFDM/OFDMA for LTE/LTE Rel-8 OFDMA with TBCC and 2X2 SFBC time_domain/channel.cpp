#include "channel.h"
#include "define.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <iomanip>
using namespace std;

#define pi 3.141592653589793
const int ofdmPeriod = Nfft+CPLen; //1280
#define delayBound 101
#define pathGainNum 7
const int symbolNum = N_SYM*N_SUBFRAME; //14*10
const int framePeriod = ofdmPeriod*symbolNum;

double tau[7] = {0e-9,30e-9,70e-9,90e-9,110e-9,190e-9,410e-9};

double Ts = 1.00/15000/1024;

double Sinc(const double &val)
{
    if(val == 0)
        return 1.00000;
    else
        return (sin(pi*val))/(pi*val);
}

void readFile(const char* filename, complex<double>** buffer)
{
    FILE* infile;
    double temp;
    infile = fopen(filename, "r");
        
    for(int i=0; i<framePeriod; i++)
        buffer[i] = new complex<double> [pathGainNum];
        
    for(int i=0; i<framePeriod; i++)
    {
        for(int j=0; j<pathGainNum; j++)
        {
            fscanf(infile,"%le",&buffer[i][j].real());
            fscanf(infile,"%le",&buffer[i][j].imag());
        }
    }       
    fclose(infile);    
}

complex<double> **FastFading(complex<double> *z, int numTxANT, int numRxANT)
{
    const int signalLen = ((int)real(z[0]))/numTxANT;
    static int symbolCount = 0;
    static complex<double>*** pathGain = new complex<double>** [numTxANT*numRxANT];
    static complex<double>**  cirBuffer = new complex<double>* [numTxANT*numRxANT];
    static int* pos = new int [numTxANT*numRxANT];
    static bool isRead = 0;

    cout.precision(14);
    if(isRead == 0)
    {
        for(int i=0; i<numTxANT*numRxANT; i++)
        {
            pos[i]=0;
            pathGain[i] = new complex<double>* [framePeriod];
            cirBuffer[i] = new complex<double> [delayBound];
        }
        if(numTxANT*numRxANT==1)
            readFile("pathGain1.txt", pathGain[0]);
        else if(numTxANT*numRxANT==2)
        {
            readFile("pathGain1.txt", pathGain[0]);
            readFile("pathGain2.txt", pathGain[1]);
        }
        else if(numTxANT*numRxANT==4)
        {
            readFile("pathGain1.txt", pathGain[0]);
            readFile("pathGain2.txt", pathGain[1]);
            readFile("pathGain3.txt", pathGain[2]);
            readFile("pathGain4.txt", pathGain[3]);                
        } 
        isRead = 1;
    }

    complex<double> **r = new complex<double>* [numRxANT];    
    for(int i=0; i<numRxANT; i++)
    {
        r[i] = new complex<double> [signalLen +1];
        r[i][0] = signalLen;
        for(int j=0; j<signalLen; j++)
        {
            r[i][j] = 0;  
        }
    }

    for(int rx=0; rx<numRxANT; rx++)
    {
        for(int tx=0; tx<numTxANT; tx++)
        {    
            complex<double> rxSig;
            complex<double> gain;
            for(int ntime=1; ntime<=signalLen; ntime++)
            {        
                rxSig = 0;
                cirBuffer[tx*2+rx][pos[tx*2+rx]]=z[tx*signalLen+ntime];
  
                // Account for multipath gain from the past to current.
                for(int ndelay=0; ndelay<delayBound; ndelay++)
                {
                    gain = 0;
                    // Influenced from the total gains ( pathGainNum )                
                    for(int ngain=0; ngain<pathGainNum; ngain++)
                    {
                        gain += pathGain[tx*2+rx][symbolCount*ofdmPeriod+(ntime-1)][ngain]*((complex<double>) Sinc((tau[ngain])/Ts-ndelay) ); 
                        //if(ntime==1280 && ngain==0 && ndelay==0)
                            //cout<<pos[tx*2+rx]<<endl;
                            //cout<<setprecision(14)<<ngain<<" "<<Sinc((tau[ngain])/Ts-ndelay)<<endl;                    
                    }
                    if(pos[tx*2+rx]-ndelay>=0)
                        rxSig += cirBuffer[tx*2+rx][pos[tx*2+rx]-ndelay]*gain;
                    else
                        rxSig += cirBuffer[tx*2+rx][delayBound+(pos[tx*2+rx]-ndelay)]*gain;
                }
 
                r[rx][ntime]+=rxSig;
                pos[tx*2+rx]++;
                if(pos[tx*2+rx]==delayBound)
                    pos[tx*2+rx] = pos[tx*2+rx]%delayBound;
            }
        }
    }
    symbolCount++;
    if(symbolCount==symbolNum)
        symbolCount=0;

    return r;
}

complex<double> **Dummy(complex<double> *z, int numTxANT, int numRxANT)
{
   const int signalLen = (int)real(z[0]);
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [signalLen +1];
   r[1] = new complex<double> [signalLen +1];
   r[0][0] = signalLen;
   r[1][0] = signalLen;
   
   // No channel effect.
   for (int j=1;j<=signalLen;j++)
   {
      r[0][j] = z[j];
      r[1][j] = z[j];
   }
   
   return r;
}

