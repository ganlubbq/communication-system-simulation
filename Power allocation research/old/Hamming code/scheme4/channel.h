void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)7/4/snr;                                         //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if (j==2) r[j]=1.8*v[j]-0.9;                          //bpsk modulation
           else  r[j]=2.03142*v[j]-1.0157; 
              
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
