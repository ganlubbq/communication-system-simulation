void rx(complex *r,int *y){
     int i,j;

     //demodulation
     for (j=0;j<k/2;j++){
         if (r[j].re>0) { y[2*j]=1; }
         else if (r[j].re<0) { y[2*j]=0; }
         else { y[2*j]=rand()&1; }
         
         if (r[j].im>0) { y[2*j+1]=1; }
         else if (r[j].im<0) { y[2*j+1]=0; }
         else { y[2*j+1]=rand()&1; }
     }
     
     
}
