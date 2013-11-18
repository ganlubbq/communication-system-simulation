float distance(float *a, float *b,int len){
      int i;
      float x=0;
      
      for (i=0;i<len;i++){
          x+=(a[i]-b[i])*(a[i]-b[i]);
      }
         
      return x;
}

void decode(float *r,int *y){
     int i,j,temp,idx;
     int code[1024][k+h];//2^k
     float sym[1024][k+h];//2^k
     float min=65536,temp1;
     
     //construct codeword
     for (i=0;i<1024;i++){
         temp=i & (2<<k)-1;
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=temp>>j & 1;
         }
         //parity bits
         
         //modulation
         for (j=0;j<k+h;j++){
             sym[i][j]=2*code[i][j]-1;
         }
         
     }
     
     //cal distance & min
     for (i=0;i<1024;i++){
         temp1=distance(r,sym[i],(k+h));
         if (temp1 <= min){
            min=temp1;
            idx=i;
         }
         
     }
       
     //decode
     for (j=0;j<k;j++){
         y[j]=code[idx][j];
     }
     
     
}
