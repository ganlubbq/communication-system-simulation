void rx(complex *r,int *y){//syndrome decoding
     int i,j,temp,sum=0;
     int z[2*N];
     int syndrome[m];
     
     //Hdd
     for (j=0;j<N;j++){
         if (r[j].re>0) { z[2*j]=1; }
         else if (r[j].re<0) { z[2*j]=0; }
         else { z[2*j]=rand()&1; }
         
         if (r[j].im>0) { z[2*j+1]=1; }
         else if (r[j].im<0) { z[2*j+1]=0; }
         else { z[2*j+1]=rand()&1; }
     }
     
     //Decode the 1st block
     for (i=0;i<m;i++){
         temp=0;
         for (j=0;j<N;j++){
             temp+=(z[j]*H[j][i]);
         }
         syndrome[i]=temp&1;
         sum+=temp;
     }

     if (sum!=0){
        for (i=0;i<N;i++){
            temp=0;
            for (j=0;j<m;j++){
                temp+=(syndrome[j]^H[i][j]);
            }
            if (temp==0){
               z[i]^=1;
               break;
            }
        }
     }
     
     //Decode the 2nd block
     sum=0;
     //derive syndrome
     for (i=0;i<m;i++){
         temp=0;
         for (j=0;j<N;j++){
             temp+=(z[N+j]*H[j][i]);
         }
         syndrome[i]=temp&1;
         sum+=temp;
     }

     if (sum!=0){
        for (i=0;i<N;i++){
            temp=0;
            for (j=0;j<m;j++){
                temp+=(syndrome[j]^H[i][j]);
            }
            if (temp==0){
               z[N+i]^=1;
               break;
            }
        }
     }
     
     for (i=0;i<k;i++){
         y[i]=z[i];
         y[k+i]=z[N+i];
     }
}
