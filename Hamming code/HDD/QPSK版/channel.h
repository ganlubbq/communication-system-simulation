void awgn(complex *x,float snr,complex *r){
     int j=0;
     float N0,sd;
    
     N0=(float)N/k/snr;                                         //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
    
     while (j<N){
           
           r[j].re=x[j].re+gaurand()*sd;
           r[j].im=x[j].im+gaurand()*sd;
        
           j++;
     }
   
}
