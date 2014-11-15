//row sum=q
//column sum=p

#define p 3
#define q 4


void rand_permute(int len,int *a){
     int i,j,x;
     int temp[len];
     
     for (i=0;i<len;i++){
         temp[i]=i;
     }
     
     for (i=0;i<len;i++){
         x=(rand()%(len-i));
         
         a[i]=temp[x];
         
         for (j=x;j<len-1-i;j++){
             temp[j]=temp[j+1];
             
         }
     }
}

void generate_H(int H[h][k+h]){
     int i,j,m,count=0,temp;
     int sequence[k+h];
     
     //initialize H to all zero
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             H[i][j]=0;
         }
     }
     
     //p submatrices with n/q blocks each
     //initialize the 1st submatrix
     for (i=0;i<(h/p);i++){
         for (j=0;j<q;j++){
             H[i][i*q+j]=1;
         }
     }
     
     A:
     
     for (i=1;i<p;i++){
         //determine column sequence
         rand_permute(k+h,sequence);
         
         //set the corresponding submatrix
         for (j=0;j<k+h;j++){
             for (m=0;m<(h/p);m++){
                 H[i*(h/p)+m][j]=H[m][sequence[j]];
             }
         }
     }
     
     /*for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d ",H[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");*/
     
     for (i=0;i<k+h-1;i++){
         for (j=i+1;j<k+h;j++){
             temp=0;
             for (m=0;m<h;m++){
                 temp+=H[m][i]*H[m][j];
             }
             if (temp > 1){
                      //system("pause"); 
                if (i>=10)printf("%d\n",i);
                goto A;
                  
             }
         }
     }
           
     for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d ",H[i][j]);
                   fprintf(fp_output,"%d ",H[i][j]);
               }
               printf("\n");
               fprintf(fp_output,"\n");
     }
     system("pause");
     
}
