float distance(float *a, float *b,int len){
      int i;
      float x=0;
      float diff;
      
      for (i=0;i<len;i++){
          //diff = a[i] - b[i];
          
          // Cancel fading effects
          diff = a[i] - b[i]*ray[i];
          
          x += diff*diff;
      }
         
      return x;
}

int code[(1<<k)][k+h];//2^k
float sym[(1<<k)][k+h];//2^k

void initialize(){
     int i,j;
     
     //construct codeword
     for (i=0;i<(1<<k);i++){
         
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=i>>j & 1;
         }
         //parity bits
         for (j=0;j<h;j++){
             code[i][k+j]=code[i][j]^code[i][j+2];
         }

         //modulation
         for (j=0;j<k+h;j++){
             //if (j==2 || j==3)sym[i][j]=2*_k1*code[i][j]-_k1;
             //else sym[i][j]=2*_k2*code[i][j]-_k2;
             
             if (j==2 || j==3)sym[i][j]=2.0*_k2*code[i][j]-_k2;   //bpsk modulation
             else if (j > 5) sym[i][j]=2.0*_k3*code[i][j]-_k3;
             else sym[i][j]=2.0*_k1*code[i][j]-_k1;
             
             
         }
     }
}    

void weight_spectrum(){
      int i,j;
      int weight,total=0,nCodeword=0;
      int d[(k+h)]={0};
      
      int w=0;
      while (w<6){
      printf("Weight = %d\n",w);
      
      for (i=0;i<(1<<k);i++){
         
         weight=0;
         for (j=0;j<k+h;j++){
             //printf("%d ",code[i][j]);
             weight+=code[i][j];
         }
         if (weight==w){
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
     
     printf("sum : \n");
     for (j=0;j<k;j++){
         printf("%d",d[j]);
     }
     printf(" ");
     for (j=k;j<k+h;j++){
         printf("%d",d[j]);
     }
     
     printf("\n\ntotal weight = %d\n",total);
     printf("number of codewords = %d\n",nCodeword);
     
     system("pause");
     w++;
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
        
     //decode
     for (j=0;j<k+h;j++){
         y[j]=code[idx][j];
     }
     
     
}
