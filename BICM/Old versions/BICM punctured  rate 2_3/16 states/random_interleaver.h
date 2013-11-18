void deinterleave1(double *);
void deinterleave2(double *);
void deinterleave3(double *);

void deinterleave(double *x1){
     int i,j;
     double *temp;
     double metric1_0[(k+m)/2],metric2_0[(k+m)/2],metric3_0[(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         metric1_0[i]=x1[4*i];
         metric2_0[i]=x1[4*i+1];
         metric3_0[i]=x1[4*i+2];
         }
     
        deinterleave1(metric1_0);
        deinterleave2(metric2_0);
        deinterleave3(metric3_0);
        
     for (i=0;i<(k+m)/2;i++){
         x1[4*i]=metric1_0[i];
         x1[4*i+1]=metric2_0[i];
         x1[4*i+2]=metric3_0[i];
         }
     }


void interleave1(int *x){
     int i,j;
     int temp[(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         temp[i]=x[record1[i]];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void deinterleave1(double *x){
     int i,j;
     double temp[(k+m)/2];
      
     for (i=0;i<(k+m)/2;i++){
         temp[record1[i]]=x[i];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void interleave2(int *x){
     int i,j;
     int temp[(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         temp[i]=x[record2[i]];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void deinterleave2(double *x){
     int i,j;
     double temp[(k+m)/2];
      
     for (i=0;i<(k+m)/2;i++){
         temp[record2[i]]=x[i];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void interleave3(int *x){
     int i,j;
     int temp[(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         temp[i]=x[record3[i]];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void deinterleave3(double *x){
     int i,j;
     double temp[(k+m)/2];
      
     for (i=0;i<(k+m)/2;i++){
         temp[record3[i]]=x[i];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
