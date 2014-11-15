
float extrinsic(float *z,int l){
      int i;
      float e,temp=1;
      
      for (i=0;i<k+h;i++){
          if (z[i]!=0 && i!=l){
             temp*=tanh(z[i]/2);
          }
      }
      
      e=log((1+temp)/(1-temp));
       
      return e;
}

//return 1 if decoding fails, return 0 if succeeds
int parity_check(int *z,int H[][k+h]){
    int i,j;
    int sum;
    
    for (i=0;i<h;i++){
        sum=0;
        for (j=0;j<k+h;j++){
            sum+=z[j]*H[i][j];
        }
        if ((sum&1) == 1) { return 1; }
    }
     
    return 0;
}

void decode(float *y,int *out, float N0){//spa decoding
     int i,j,count=0,count2,z[k+h]={0};
     int H[h][k+h]={1,0,1,0,0,0,1,0,0,0,
                    0,1,0,1,0,0,0,1,0,0,
                    0,0,1,0,1,0,0,0,1,0,
                    0,0,0,1,0,1,0,0,0,1};
     int G[h][k+h];
     float Y[h][k+h]={0},Z[h][k+h]={0},E[h][k+h]={0},tempE[h][k+h]={0};
     float e[k+h]={0},r[k+h]={0},temp1;
     
     
     generate_H(H);
     
     //get_G(H,G);
     
     //initialize r0[]
     for (i=0;i<k+h;i++){
         r[i]=(4/N0)*y[i];
     }
     
     //initialize Y
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             Y[i][j]=H[i][j] * (4/N0)*y[j];
             Z[i][j]=Y[i][j];
         }
     }
     
     /*for (i=0;i<k+h;i++){//run column
               printf("% .1f ",(4/N0)*y[i]);
     }
     system("pause");*/
           
     /*for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("% .1f ",Y[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");*/
     
     while (count<Imax){
           //cal tempE
           for (i=0;i<k+h;i++){//run column
               for (j=0;j<h;j++){//row search (who checks on me)
                   if (H[j][i] != 0){
                      tempE[j][i]=extrinsic(Z[j],i);//i corresponds to "l"
                   }
               }
           }
           
           //cal total extrinsic correspond to an "l"
           for (i=0;i<k+h;i++){//run column
               e[i]=0;
               for (j=0;j<h;j++){//row
                   if (H[j][i] != 0){
                      e[i]+=tempE[j][i];
                   }
               }
           }

           //derive E
           for (i=0;i<h;i++){//row
               for (j=0;j<k+h;j++){//column
                   if (H[i][j] != 0) { E[i][j]=e[j]-tempE[i][j]; }
               }
           }
     
           //update Z
           for (i=0;i<h;i++){//row
               for (j=0;j<k+h;j++){//column
                   Z[i][j]=Y[i][j]+E[i][j];
               }
           }
     
           //get r[]
           for (i=0;i<k+h;i++){
               r[i]+=e[i];
           }
     
           //get z[]
           for (i=0;i<k+h;i++){
               if (r[i]>0) { z[i]=1; }
               else if (r[i]<0) { z[i]=0; }
               else { z[i]=rand()&1; }
           }
     
           //test for stop
           if ( parity_check(z,H) == 0 )  { break; } //will return 1 if decoding fails, return 0 if succeeds
           
           count++;
           
           /*for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("% .1f ",E[i][j]);
                   
               }
               printf("\n");
           }
           system("pause");*/
           
           /*for (i=0;i<k+h;i++){//run column
               printf("% .1f ",r[i]);
           }
           system("pause");*/
     }
     
    
     for (i=0;i<k;i++){
         out[i]=z[i];
     }
     
}





