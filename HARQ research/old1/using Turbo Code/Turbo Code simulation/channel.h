void AWGN(int *x, int len, float snr, float *r)
{
    float N0=3.0/snr;//SNR = (3*Eb)/N0
    float sd=sqrt(N0/2.0);//standard deviation of gaussian r.v.
   
    int j=0;
    while (j<len){
          r[j]=2.0*x[j]-1.0;
              
          r[j] += gaurand()*sd;//add noise     
          
          //ray[j]=rayrand();
          //r[j] = r[j]*ray[j];                              
          j++;
    }
}
