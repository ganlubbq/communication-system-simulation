struct complex{
       float re;
       float im;
       };

void tx(int *u,complex *x){//also generate random data
     int i,j,idx;
     int v[k];//encoded bits
     
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
  
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }
     
     //modulation
     for (i=0;i<k/2;i++){
         x[i].re=2*v[2*i]-1;
         x[i].im=2*v[2*i+1]-1;
     }

}
     
