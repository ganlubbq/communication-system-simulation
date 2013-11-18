void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=2/snr;                                         //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if(j<k) r[j]=2.2*v[j]-1.1;                          //bpsk modulation
           else r[j]=1.77764*v[j]-0.88882;
              
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
