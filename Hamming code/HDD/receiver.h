void rx(float *r,int *y){//syndrome decoding
     int i,j,temp,sum=0;
     int z[N];
     int syndrome[m];
     
     //hdd
     for (i=0;i<N;i++){
         if (r[i]>0) { z[i]=1; }
         else if (r[i]<0) { z[i]=0; }
         else { z[i]=rand()&1; }
     }
     
     //derive syndrome
     for (i=0;i<m;i++){
         temp=0;
         for (j=0;j<N;j++){
             temp+=(z[j]*H[j][i]);
         }
         syndrome[i]=temp&1;
         sum+=temp;
     }
     
     /*for (j=0;j<m;j++){
                printf("%d",syndrome[j]);
            }
            system("pause");*/
     
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
     
     
     for (i=0;i<k;i++){
         y[i]=z[i];
     }
}
