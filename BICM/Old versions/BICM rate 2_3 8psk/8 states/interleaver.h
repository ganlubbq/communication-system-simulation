void interleave1(int *);
void deinterleave1(float *);
void interleave2(int *);
void deinterleave2(float *);
void interleave3(int *);
void deinterleave3(float *);

void interleave(int *x1){
     int i,j;
     int metric1_0[(k+m)/2],metric2_0[(k+m)/2],metric3_0[(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         metric1_0[i]=x1[3*i];
         metric2_0[i]=x1[3*i+1];
         metric3_0[i]=x1[3*i+2];
         }
     
        interleave1(metric1_0);
        interleave2(metric2_0);
        interleave3(metric3_0);
        
     for (i=0;i<(k+m)/2;i++){
         x1[3*i]=metric1_0[i];
         x1[3*i+1]=metric2_0[i];
         x1[3*i+2]=metric3_0[i];
         }
     }

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
     int i,j,temp[(k+m)/2];
     
     for (i=0;i<40;i++){
         for (j=0;j<50;j++){
             temp[50*i+j]=x[40*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave1(float *x){
     int i,j;
     float temp[(k+m)/2];
     for (i=0;i<40;i++){
         for (j=0;j<50;j++){
             temp[40*j+i]=x[50*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     
     }
     
void interleave2(int *x){
     int i,j,temp[(k+m)/2];
    
     for (i=0;i<80;i++){
         for (j=0;j<25;j++){
             temp[25*i+j]=x[80*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave2(float *x){
     int i,j;
     float temp[(k+m)/2];
     
     for (i=0;i<80;i++){
         for (j=0;j<25;j++){
             temp[80*j+i]=x[25*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     
     }
     
void interleave3(int *x){
     int i,j,temp[(k+m)/2];
     
     for (i=0;i<16;i++){
         for (j=0;j<125;j++){
             temp[125*i+j]=x[16*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave3(float *x){
     int i,j;
     float temp[(k+m)/2];
     
     for (i=0;i<16;i++){
         for (j=0;j<125;j++){
             temp[16*j+i]=x[125*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     }
