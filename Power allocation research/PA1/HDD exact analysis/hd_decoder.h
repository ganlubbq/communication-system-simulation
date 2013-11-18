int distance(int *a, int *b,int len){
      int i;
      int count=0;
      
      for (i=0;i<len;i++){
          count+=(a[i]+b[i])&1;
      }
         
      return count;
}

void bound(){
     int i,j,idx;
     int r_hd[k+h];
     int code[(1<<k)][k+h];
     int received[(1<<(k+h))][k+h];
     int decoded[(1<<(k+h))][k+h];
     int min=65536,temp1,temp2;

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
  
     /*for (i=0;i<(1<<(k+h));i++){
         for (j=0;j<k+h;j++){
             printf("%d",received[i][j]);
         }
         printf("=> ");
         for (j=0;j<k+h;j++){
             printf("%d",decoded[i][j]);
         }
         
         printf("\n");
         getchar();
         
     }*/
     for (i=0;i<(1<<(k+h));i++){
         
         //temp1=0;
         for (j=0;j<k+h;j++){
             //temp1+=received[i][j];
             
             //Pe + Pc = 1
             if (received[i][j]==1) { printf("Pe*");fprintf(fp_output,"Pe*"); }
             else { printf("Pc*");fprintf(fp_output,"Pc*"); }
         }
         
         temp2=0;
         for (j=0;j<k;j++){
             temp2+=decoded[i][j];
         }
         
         printf("(%d/K) +\n",temp2);
         fprintf(fp_output,"(%d/K) + ...\n",temp2);
     
         //getchar();
         
     }
     
}
