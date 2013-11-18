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
int Ht[(k+h)][h]={
0,0,1,1,1,
1,1,0,0,0,
0,1,1,0,1,
1,0,0,1,0,
0,0,1,0,1,
0,1,1,1,1,
1,0,1,1,1,
1,1,0,1,1,
1,1,1,0,0,
0,1,1,1,0,
//-------
1,0,0,0,0,
0,1,0,0,0,
0,0,1,0,0,
0,0,0,1,0,
0,0,0,0,1
};


void initialize(){
     int i,j,l,temp;
     
     //construct codeword
     for (i=0;i<(1<<k);i++){
         
         //systematic bits
         for (j=0;j<k;j++){
             code[i][k-1-j]=i>>j & 1;
         }
         
         //parity bits
         for (j=0;j<h;j++){
             temp = 0;
             for (l=0;l<k;l++){
                 temp^=(code[i][l]*Ht[l][j]);
             }
             code[i][k+j]=temp;
         }
         
         //modulation
         for (j=0;j<k+h;j++){
             sym[i][j]=2.0*code[i][j]-1.0;
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

void weight_spectrum_2(){
      int i,j,temp;
      int weight,total=0,nCodeword=0;
      int d[(k+h)]={0};
      int g[6]={0};//5 is the group number
      
      int w=0;
      while (w<5){
      printf("Weight = %d\n",w);
      
      for (i=0;i<(1<<k);i++){
         
         weight=0;
         for (j=0;j<k+h;j++){
             weight+=code[i][j];
         }
         if (weight==w){
             for (j=0;j<6;j++){
                 g[j]=0;
             }
             
             if(code[i][0]==1) { d[0]++; g[3]++;}
             if(code[i][1]==1) { d[1]++; g[2]++;}
             if(code[i][2]==1) { d[2]++; g[1]++;}
             if(code[i][3]==1) { d[3]++; g[2]++;}
             if(code[i][4]==1) { d[4]++; g[3]++;}
             if(code[i][5]==1) { d[5]++; g[3]++;}
             if(code[i][6]==1) { d[6]++; g[2]++;}
             if(code[i][7]==1) { d[7]++; g[0]++;}
             if(code[i][8]==1) { d[8]++; g[2]++;}
             if(code[i][9]==1) { d[9]++; g[1]++;}
             if(code[i][10]==1) { d[10]++; g[5]++;}
             if(code[i][11]==1) { d[11]++; g[5]++;}
             if(code[i][12]==1) { d[12]++; g[4]++;}
             if(code[i][13]==1) { d[13]++; g[5]++;}
             if(code[i][14]==1) { d[14]++; g[4]++;}
             temp=0;
             for (j=0;j<k;j++){
                 if(code[i][j]==1) { temp++; }
                // printf("%d",code[i][j]);
             }
             printf(" ");
             for (j=k;j<k+h;j++){
                // if(code[i][j]==1) { d[j]++; g[4]++; }
               //  printf("%d",code[i][j]);
             } 
            nCodeword++;          
            total+=weight;
            
            
            printf("%d/10*P2(",temp);
            for (j=0;j<6;j++){
                printf("%d",g[j]);
                if (j != 5) { printf(","); }
            }
            printf(")+ ...");
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
