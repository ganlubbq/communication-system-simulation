void Minterleave(double *x){
     int i,j;
     double temp1[k/2],temp2[k/2];

     for (i=0;i<k/2;i++){
         temp1[i]=x[2*record1[i]];
         temp2[i]=x[2*record2[i]+1];
         }
    
     for (i=0;i<k/2;i++){
         x[2*i]=temp1[i];
         x[2*i+1]=temp2[i];
         }
     
     }

void interleave(int *x){
     int i,j;
     int temp1[k/2],temp2[k/2];

     for (i=0;i<k/2;i++){
         temp1[i]=x[2*record1[i]];
         temp2[i]=x[2*record2[i]+1];
         }
    
     for (i=0;i<k/2;i++){
         x[2*i]=temp1[i];
         x[2*i+1]=temp2[i];
         }
     
     }
     
void deinterleave(double *x){
     int i,j;
     double temp1[k/2],temp2[k/2];
      
     for (i=0;i<k/2;i++){
         temp1[record1[i]]=x[2*i];
         temp2[record2[i]]=x[2*i+1];
         }
     
     for (i=0;i<k/2;i++){
         x[2*i]=temp1[i];
         x[2*i+1]=temp2[i];
         }
     
     }
