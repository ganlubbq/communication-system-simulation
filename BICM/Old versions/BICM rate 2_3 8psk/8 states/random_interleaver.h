void deinterleave1(float *);
void deinterleave2(float *);
void deinterleave3(float *);

void deinterleave(float *x,float *y){
     int i,j;
     float *temp;
     float metric1_0[(k+m)/2],metric2_0[(k+m)/2],metric3_0[(k+m)/2];
     float metric1_1[(k+m)/2],metric2_1[(k+m)/2],metric3_1[(k+m)/2];
     
     
     for (i=0;i<(k+m)/2;i++){
         metric1_0[i]=x[3*i];
         metric2_0[i]=x[3*i+1];
         metric3_0[i]=x[3*i+2];
         metric1_1[i]=y[3*i];
         metric2_1[i]=y[3*i+1];
         metric3_1[i]=y[3*i+2];
         }
     
        deinterleave1(metric1_0);
        deinterleave1(metric1_1);
        deinterleave2(metric2_0);
        deinterleave2(metric2_1);
        deinterleave3(metric3_0);
        deinterleave3(metric3_1);
     
     for (i=0;i<(k+m)/2;i++){
         x[3*i]=metric1_0[i];
         x[3*i+1]=metric2_0[i];
         x[3*i+2]=metric3_0[i];
         y[3*i]=metric1_1[i];
         y[3*i+1]=metric2_1[i];
         y[3*i+2]=metric3_1[i];
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
     
void deinterleave1(float *x){
     int i,j;
     float temp[(k+m)/2];
      
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
     
void deinterleave2(float *x){
     int i,j;
     float temp[(k+m)/2];
      
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
     
void deinterleave3(float *x){
     int i,j;
     float temp[(k+m)/2];
      
     for (i=0;i<(k+m)/2;i++){
         temp[record3[i]]=x[i];
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
