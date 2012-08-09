struct complex{
       float re;
       float im;
       };

void tx(int *u,complex *x){//also generate random data
     int i,j,temp;
     int v[2*N];
     
     for (i=0;i<2*k;i++){
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
     
     for (i=0;i<k;i++){
         v[N+i]=u[k+i];
     }
     
     for (i=0;i<m;i++){
         temp=0;
         for (j=0;j<k;j++){
             temp+=v[N+j]*H[j][i];
         }
         v[N+k+i]=temp&1;
     }
     
     //modulation
     for (i=0;i<N;i++){
         x[i].re=2*v[2*i]-1;
         x[i].im=2*v[2*i+1]-1;
     }

}
     
