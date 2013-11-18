void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)5/3/snr;                                         //SNR = avgEb/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if (j==2 || j==3)r[j]=2.4*v[j]-1.2;                          //bpsk modulation
           else r[j]=1.886796*v[j]-0.943398;
              
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
