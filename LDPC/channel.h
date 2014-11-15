void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)(k+h)/k/snr;                                         //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           r[j]=2*v[j]-1;                          //bpsk modulation
              
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
