
float extrinsic(float *z,int l,int count){
      int i;
      double temp=1.0;
      float e;

      for (i=0;i<k+h;i++){
          if (z[i]!=0 && i!=l){
             temp*=tanh(z[i]/2);
          }
      }
      
      if(temp == -1.0){ e=-50; }
      else { e=log((1+temp)/(1-temp)); }
      
      //e=log((1+temp)/(1-temp));
      
      return e;
}

//return 1 if decoding fails, return 0 if succeeds
int parity_check(int *z,int **H){
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

void decode(float *y,int *out, float N0,int **H){//spa decoding
     int i,j,l,count=0,*z;
     float **Y,**Z,**E,**tempE;
     float *e,*r;
     
     Y=(float **)malloc(h*sizeof(float *));
     Z=(float **)malloc(h*sizeof(float *));
     E=(float **)malloc(h*sizeof(float *));
     tempE=(float **)malloc(h*sizeof(float *));

     for (i=0;i<h;i++){
         *(Y+i)=(float *)malloc((k+h)*sizeof(float));
         *(Z+i)=(float *)malloc((k+h)*sizeof(float));
         *(E+i)=(float *)malloc((k+h)*sizeof(float));
         *(tempE+i)=(float *)malloc((k+h)*sizeof(float));
     }
     
     z=(int *)malloc((k+h)*sizeof(int));
     e=(float *)malloc((k+h)*sizeof(float));
     r=(float *)malloc((k+h)*sizeof(float));
     
     
     //initialize all 2-d array to zero
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             Y[i][j]=0;
             Z[i][j]=0;
             E[i][j]=0;
             tempE[i][j]=0;
         }
     }

     //initialize r0[]
     for (i=0;i<k+h;i++){
         r[i]=(4/N0)*y[i];
         e[i]=0;
     }
     
     //initialize Y
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             Y[i][j]=H[i][j] * (4/N0)*y[j];
             Z[i][j]=Y[i][j];
         }
     }
     
     while (count<Imax){
           
           //cal tempE
           for (i=0;i<k+h;i++){//run column
               for (j=0;j<h;j++){//row search (who checks on me)
                   if (H[j][i] != 0){
                      
                      tempE[j][i]=extrinsic(Z[j],i,count);//i corresponds to "l"
                      
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
           
          /* printf("%dth iteration\n",count);
           
           system("pause");
           
           for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("% .1f ",E[i][j]);
                   
               }
               printf("\n");
           }
           system("pause");
           
           for (i=0;i<k+h;i++){//run column
               printf("% .1f ",r[i]);
           }
           system("pause");*/
     }
     
    
     for (i=0;i<k;i++){
         out[i]=z[i];
     }
     
     for (i=0;i<h;i++){
        free(*(Y+i));
        free(*(Z+i));
        free(*(E+i));
        free(*(tempE+i));
    }

    free(Y);free(Z);free(E);free(tempE);
    free(z);free(e);free(r);
}





