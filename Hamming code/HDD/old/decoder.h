int distance(int *a, int *b,int len){
      int i;
      int count=0;
      
      for (i=0;i<len;i++){
          count+=(a[i]+b[i])&1;
      }
         
      return count;
}

void decode(float *r,int *y){
     int i,j,temp,idx,temp1;
     int code[16][k+h];//2^k
     int r_hd[k+h];
     float min=65536;
     
     //construct codeword
     for (i=0;i<16;i++){
         temp=i & (2<<k)-1;
         
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=temp>>j & 1;
         }
         //parity bits
         code[i][k+0]=code[i][0]^code[i][2]^code[i][3];
         code[i][k+1]=code[i][0]^code[i][1]^code[i][2];
         code[i][k+2]=code[i][1]^code[i][2]^code[i][3];

     }
     
     for (j=0;j<k+h;j++){
         if (r[j]>0) { r_hd[j]=1; }
         else if (r[j]<0) { r_hd[j]=0; }
         else { r_hd[j]=rand()&1; }
     }
         
     //cal hamming distance & min
     for (i=0;i<16;i++){
         temp1=distance(r_hd,code[i],(k+h));
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
