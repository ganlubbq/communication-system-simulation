float distance(float *a, float *b,int len){
      int i;
      float x=0;
      
      for (i=0;i<len;i++){
          x+=(a[i]-b[i])*(a[i]-b[i]);
      }
         
      return x;
}

float sym[(1<<k)][k+h];//2^k
int code[(1<<k)][k+h];//2^k

void initialize(){
     int i,j,idx;
     
     //construct codeword
     for (i=0;i<(1<<k);i++){
         
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=i>>j & 1;
         }
         //parity bits
         code[i][k+0]=code[i][2]^code[i][4]^code[i][5]^code[i][7]^code[i][8]^code[i][9]^code[i][10];
         code[i][k+1]=code[i][1]^code[i][3]^code[i][5]^code[i][6]^code[i][7]^code[i][8]^code[i][10];
         code[i][k+2]=code[i][0]^code[i][3]^code[i][4]^code[i][6]^code[i][7]^code[i][9]^code[i][10];
         code[i][k+3]=code[i][0]^code[i][1]^code[i][2]^code[i][6]^code[i][8]^code[i][9]^code[i][10];
     
         //modulation
         for (j=0;j<k+h;j++){
             sym[i][j]=2*code[i][j]-1;
         }
         
     }
     
}     
     
void decode(float *r,int *y){
     int i,j,idx;
     float min=65536,temp1;
     
     
     
     //cal distance & min
     for (i=0;i<(1<<k);i++){
         temp1=distance(r,sym[i],(k+h));
         if (temp1 <= min){
            min=temp1;
            idx=i;
         }
         
     }
     /*
      int weight,total=0,nCodeword=0;
      int d[(k+h)]={0};
      
      for (i=0;i<(1<<k);i++){
         
         weight=0;
         for (j=0;j<k+h;j++){
             //printf("%d ",code[i][j]);
             weight+=code[i][j];
         }
         if (weight==4){
             for (j=0;j<k;j++){
                 if(code[i][j]==1) { d[j]++; }
                 printf("%d",code[i][j]);
             }
             printf(" ");
             for (j=k;j<k+h;j++){
                 if(code[i][j]==1) { d[j]++; }
                 printf("%d",code[i][j]);
             } 
            nCodeword++;          
            total+=weight;
            printf("\n");
         }
         
     }
     
     for (j=0;j<k;j++){
         printf("%d",d[j]);
     }
     printf(" ");
     for (j=k;j<k+h;j++){
         printf("%d",d[j]);
     }
     
     printf("\n\ntotal weight = %d\n",total);
     printf("number of codewords = %d\n",nCodeword);
     
     system("pause");*/
     
     //decode
     for (j=0;j<k;j++){
         y[j]=code[idx][j];
     }
     
     
}
