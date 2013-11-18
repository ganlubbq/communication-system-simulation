void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
    
     N0=(float)5/3/snr;                                         //SNR = avgEb/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if (j==2 || j==3)r[j]=1.2*v[j]-0.6;                          //bpsk modulation
           else r[j]=2.1540659*v[j]-1.07703296;
              
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
