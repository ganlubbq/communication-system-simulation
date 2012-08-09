void awgn(float *x,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)N/k/snr;                                         //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
    
     while (j<N){
      
           r[j]=x[j]+gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
