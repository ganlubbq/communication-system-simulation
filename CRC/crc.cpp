#include "crc.h"

const int r = 8;                                  // CRC-r
const int GCRC = 0x07;                            // CRC generator

int gcrc[r+1];                               // generator sequence

void generator_enumerate()
{
     int temp1=GCRC;
     int check = 1 << (r+1);
     
     //generator check
     if ( temp1>=check ) temp1-=check;

     for (int i=0;i<r+1;i++)
     {
         gcrc[i] = temp1 >> (r-i);
         temp1 = temp1 & ((1<<(r-i))-1);
     }
     gcrc[0]=1;
}
     
int *crc(int *u)
{
     static int hasInit = 0;
     if (hasInit == 0)
     {
        generator_enumerate();
        hasInit = 1;   
     }
     
     int *reg = new int [r];
     for (int i=0;i<r;i++) { reg[i] = 0; }
     
     int FL = u[0];
     int *u_crc = new int [FL+r+1];
     u_crc[0] = FL+r;
     
     for (int i=1;i<=FL;i++) { u_crc[i] = u[i]; }
     for (int i=FL+1;i<=FL+r;i++) { u_crc[i] = 0; }

     for(int i=0;i<FL+r;i++)
     {
         int temp=reg[0];             
         for(int j=0;j<r-1;j++)
         {
            reg[j] = reg[j+1]^(temp*gcrc[j+1]);
         }
         
         reg[r-1] = u_crc[i+1]^(temp*gcrc[r]);
     }
     
     for(int i=FL;i<FL+r;i++)
     {
         u_crc[i+1] = reg[i-FL];
     }
     
     delete [] reg;
     
     return u_crc;
}
