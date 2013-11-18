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
         code[i][k+0]=code[i][0]^code[i][3];
         code[i][k+1]=code[i][2]^code[i][5]^code[i][6]^code[i][8];
         code[i][k+2]=code[i][1]^code[i][4];
         code[i][k+3]=code[i][0]^code[i][6]^code[i][7]^code[i][9];
         code[i][k+4]=code[i][0]^code[i][2];
         code[i][k+5]=code[i][3]^code[i][5]^code[i][8]^code[i][9];
         code[i][k+6]=code[i][1]^code[i][4];
         code[i][k+7]=code[i][2]^code[i][6]^code[i][8]^code[i][9];
         code[i][k+8]=code[i][6]^code[i][7];
         code[i][k+9]=code[i][1]^code[i][4]^code[i][5]^code[i][7];
     
         //modulation
         for (j=0;j<k+h;j++){
             if (j<k) sym[i][j]=1.8*code[i][j]-0.9;
             else sym[i][j]=2.18174*code[i][j]-1.09087;
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
