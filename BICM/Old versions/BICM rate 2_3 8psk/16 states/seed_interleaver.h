int seed1=13;
int seed2=43;
int seed3=97;

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
     int i,j,count=0,row;
     int temp[(k+m)/2];
      
     if((k+m)/2%seed1==0) row=(k+m)/2/seed1;
     else row=(k+m)/2/seed1+1;
     
     for (i=0;i<seed1;i++){
         for (j=0;j<row;j++){
             if(seed1*j+i<(k+m)/2)
                temp[row*i+j-count]=x[seed1*j+i];
             else count++;
             }
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave1(float *x){
     int i,j,row,count=0;
     float temp[(k+m)/2];
     
     if((k+m)/2%seed1==0) row=(k+m)/2/seed1;
     else row=(k+m)/2/seed1+1;
     
     for (i=0;i<seed1;i++){
         for (j=0;j<row;j++){
             
             if(seed1*j+i<(k+m)/2)
             temp[seed1*j+i]=x[row*i+j-count];
             else count++;
             
             }
         }
          
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void interleave2(int *x){
     int i,j,count=0,row;
     int temp[(k+m)/2];
     
     if((k+m)/2%seed2==0) row=(k+m)/2/seed2;
     else row=(k+m)/2/seed2+1;
     
     for (i=0;i<seed2;i++){
         for (j=0;j<row;j++){
             if(seed2*j+i<(k+m)/2)
                temp[row*i+j-count]=x[seed2*j+i];
             else count++;
             }
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave2(float *x){
     int i,j,count=0,row;
     float temp[(k+m)/2];
    
     if((k+m)/2%seed2==0) row=(k+m)/2/seed2;
     else row=(k+m)/2/seed2+1;
     
     for (i=0;i<seed2;i++){
         for (j=0;j<row;j++){
             
             if(seed2*j+i<(k+m)/2)
             temp[seed2*j+i]=x[row*i+j-count];
             else count++;
             
             }
         }
          
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }
     
void interleave3(int *x){
     int i,j,count=0,row;
     int temp[(k+m)/2];
     
     if((k+m)/2%seed3==0) row=(k+m)/2/seed3;
     else row=(k+m)/2/seed3+1;
     
     for (i=0;i<seed3;i++){
         for (j=0;j<row;j++){
             if(seed3*j+i<(k+m)/2)
                temp[row*i+j-count]=x[seed3*j+i];
             else count++;
             }
         }
     
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
     }

void deinterleave3(float *x){
     int i,j,count=0,row;
     float temp[(k+m)/2];
     
     if((k+m)/2%seed3==0) row=(k+m)/2/seed3;
     else row=(k+m)/2/seed3+1;
     
     for (i=0;i<seed3;i++){
         for (j=0;j<row;j++){
             
             if(seed3*j+i<(k+m)/2)
             temp[seed3*j+i]=x[row*i+j-count];
             else count++;
             
             }
         }
          
     for (i=0;i<(k+m)/2;i++){
         x[i]=temp[i];
         }
     
    }
