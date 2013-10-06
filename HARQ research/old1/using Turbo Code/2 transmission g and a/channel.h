void AWGN(float *x, int len, float snr, float *r, float* noiseBuff)
{
    float N0=3.0/snr;//SNR = (3*Eb)/N0
    float sd=sqrt(N0/2.0);//standard deviation of gaussian r.v.
   
    int j=0;
    while (j<len){
          r[j]=x[j];
              
          float n = gaurand()*sd;
 
          r[j] += n;//add noise     
          
          noiseBuff[j] = n;         
          
          //ray[j]=rayrand();
          //r[j] = r[j]*ray[j];                              
          j++;
    }
}
