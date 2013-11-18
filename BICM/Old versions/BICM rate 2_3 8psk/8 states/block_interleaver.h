void interleave(int *x){
     int i,j;
     int temp[(k+m)/2*3];
     
     for (i=0;i<25;i++){
         for (j=0;j<240;j++){
             temp[240*i+j]=x[25*j+i];
             }
         }
     
     for (i=0;i<6000;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave(float *x,float *y){
     int i,j;
     double temp[(k+m)/2*3];
     for (i=0;i<25;i++){
         for (j=0;j<240;j++){
             temp[25*j+i]=x[240*i+j];
             }
         }
     
     for (i=0;i<6000;i++){
         x[i]=temp[i];
         }
     
     for (i=0;i<25;i++){
         for (j=0;j<240;j++){
             temp[25*j+i]=y[240*i+j];
             }
         }
     
     for (i=0;i<6000;i++){
         y[i]=temp[i];
         }
     }
     
