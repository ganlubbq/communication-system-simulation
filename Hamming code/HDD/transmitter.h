void tx(int *u,float *x){//also generate random data
     int i,j,temp;
     int v[N];
     
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
     
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }
     
     for (i=0;i<m;i++){
         temp=0;
         for (j=0;j<k;j++){
             temp+=v[j]*H[j][i];
         }
         v[k+i]=temp&1;
     }
     
     //modulation
     for (i=0;i<N;i++){
         x[i]=2*v[i]-1;                          //bpsk modulation
     }

}
     
