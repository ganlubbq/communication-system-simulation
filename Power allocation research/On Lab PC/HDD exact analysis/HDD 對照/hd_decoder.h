int distance(int *a, int *b,int len){
      int i;
      int count=0;
      
      for (i=0;i<len;i++){
          count+=(a[i]+b[i])&1;
      }
         
      return count;
}

int received[(1<<(k+h))][k+h];
int decoded[(1<<(k+h))][k+h];
int code[(1<<k)][k+h];//2^k
     
void initialize(){
     int i,j,temp1,idx;
     int min=65536;
  
     //construct codeword
     for (i=0;i<(1<<k);i++){
         
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=i>>j & 1;
         }
         //parity bits
         code[i][k+0]=code[i][0]^code[i][2];
         code[i][k+1]=code[i][1]^code[i][3];
         code[i][k+2]=code[i][2]^code[i][4];
         code[i][k+3]=code[i][3]^code[i][5];
     }
     
     //Construt the whole possible received sequence
     for (i=0;i<(1<<(k+h));i++){
         
         for (j=0;j<k+h;j++){
             received[i][k+h-1-j]=i>>j & 1;
         }
     }
     
     //Construt the decoded sequence
     for (i=0;i<(1<<(k+h));i++){
         min=65536;
         
         //cal hamming distance & min
         for (j=0;j<(1<<k);j++){
             temp1=distance(received[i],code[j],(k+h));
             if (temp1 <= min){
                min=temp1;
                idx=j;
             }
         }
         
         for (j=0;j<k+h;j++){
             decoded[i][j]=code[idx][j];
         }
     }

}

void decode(float *r,int *y){
     int i,j,temp1,idx;
     int r_hd[k+h];
     int min=65536;
     int nMin;
     int minArray[(1<<k)];
     
     for (j=0;j<k+h;j++){
         if (r[j]>0) { r_hd[j]=1; }
         else if (r[j]<0) { r_hd[j]=0; }
         else { r_hd[j]=rand()&1; }
     }
      
       
     //cal hamming distance & min
     for (i=0;i<(1<<k);i++){
         temp1=distance(r_hd,code[i],(k+h));
         if (temp1 <= min){
            min=temp1;
            idx=i;
         }
         
     }
     
     /*
     //Randomly choose the sequence with equal hamming d
     nMin=0;
     for (i=0;i<(1<<k);i++){
         temp1=distance(r_hd,code[i],(k+h));
         if (temp1 == min){
            minArray[nMin]=i;
            nMin++;
         }
     }
     
     temp1=rand()%nMin;
     idx=minArray[temp1];*/

     //decode
     for (j=0;j<k;j++){
         y[j]=code[idx][j];
     }
     
    /* min=0;
     for (i=0;i<(1<<(k+h));i++){
         temp1=distance(r_hd,received[i],(k+h));
         if (temp1 == 0){
            min++;
            idx=i;
         }
        
     }

     //decode
     for (j=0;j<k;j++){
         y[j]=decoded[idx][j];
     }
     */
     
     
}
