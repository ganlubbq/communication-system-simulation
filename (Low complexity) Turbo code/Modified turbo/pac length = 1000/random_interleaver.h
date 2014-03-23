void Minterleave(float *x1){
     int i,j;
     float temp1[k];
     
     for (i=0;i<k;i++){
         temp1[i]=x1[record[i]];
         }
     
     for (i=0;i<k;i++){
         x1[i]=temp1[i];
         }
     }

void interleave(int *x){
     int i,j;
     int temp[k];
     
     for (i=0;i<k;i++){
         temp[i]=x[record[i]];
         }
     
     for (i=0;i<k;i++){
         x[i]=temp[i];
         }
     
     }
     
void deinterleave(int *x1){
     int i,j;
     int temp1[k];
      
     for (i=0;i<k;i++){
         temp1[record[i]]=x1[i];
         }
     
     for (i=0;i<k;i++){
         x1[i]=temp1[i];
         }
     
     }
      
void Mdeinterleave(float *x1){
     int i,j;
     float temp1[k];
      
     for (i=0;i<k;i++){
         temp1[record[i]]=x1[i];
         }
     
     for (i=0;i<k;i++){
         x1[i]=temp1[i];
         }
     
     }
