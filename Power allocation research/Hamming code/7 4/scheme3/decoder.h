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
     int code[16][k+h];//2^k
     float sym[16][k+h];//2^k
     float min=65536,temp1;
     
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
         
     
         //modulation
         for (j=0;j<k+h;j++){
             if (j==2) sym[i][j]=2.2*code[i][j]-1.1;                          //bpsk modulation
             else  sym[i][j]=1.964688*code[i][j]-0.982344; 
         }
         
     }
     
     //cal distance & min
     for (i=0;i<16;i++){
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
