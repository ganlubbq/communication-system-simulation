void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)5/3/snr;                                         //SNR = avgEb/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if (j<k)r[j]=1.8*v[j]-0.9;                          //bpsk modulation
           else r[j]=2.267157*v[j]-1.133578;;
           
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
