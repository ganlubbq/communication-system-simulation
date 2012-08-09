void decode(float *r,int *y){
     int i,j,temp,idx;

     //decode
     for (j=0;j<k;j++){
         if (r[j]>0) { y[j]=1; }
         else if (r[j]<0) { y[j]=0; }
         else { y[j]=rand()&1; }
     }
     
     
}
