void interleave1(int *);
void interleave2(int *);
void interleave3(int *);
void deinterleave1(double *);
void deinterleave2(double *);
void deinterleave3(double *);

void interleave(int *x1){
     int i,j;
     int metric1_0[(k+m)/3*2],metric2_0[(k+m)/3*2],metric3_0[(k+m)/3*2];
     
     for (i=0;i<(k+m)/3*2;i++){
         metric1_0[i]=x1[3*i];
         metric2_0[i]=x1[3*i+1];
         metric3_0[i]=x1[3*i+2];
         }
     
        interleave1(metric1_0);
        interleave2(metric2_0);
        interleave3(metric3_0);
        
     for (i=0;i<(k+m)/3*2;i++){
         x1[3*i]=metric1_0[i];
         x1[3*i+1]=metric2_0[i];
         x1[3*i+2]=metric3_0[i];
         }
     }
     
     
void deinterleave(double *x1){
     int i,j;
     double metric1_0[(k+m)/3*2],metric2_0[(k+m)/3*2],metric3_0[(k+m)/3*2];
     
     for (i=0;i<(k+m)/3*2;i++){
         metric1_0[i]=x1[3*i];
         metric2_0[i]=x1[3*i+1];
         metric3_0[i]=x1[3*i+2];
         }
     
        deinterleave1(metric1_0);
        deinterleave2(metric2_0);
        deinterleave3(metric3_0);
        
     for (i=0;i<(k+m)/3*2;i++){
         x1[3*i]=metric1_0[i];
         x1[3*i+1]=metric2_0[i];
         x1[3*i+2]=metric3_0[i];
         }
     }


void interleave1(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/3*2*sizeof(int));
     
     for (i=0;i<18;i++){
         for (j=0;j<19;j++){
             temp[19*i+j]=x[18*j+i];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave1(double *x){
     int i,j;
     double *temp;
     temp=(double *) malloc((k+m)/3*2*sizeof(double));
     
     for (i=0;i<18;i++){
         for (j=0;j<19;j++){
             temp[18*j+i]=x[19*i+j];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave2(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/3*2*sizeof(int));
     
     for (i=0;i<38;i++){
         for (j=0;j<9;j++){
             temp[9*i+j]=x[38*j+i];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave2(double *x){
     int i,j;
     double *temp;
     temp=(double *) malloc((k+m)/3*2*sizeof(double));
     
     for (i=0;i<38;i++){
         for (j=0;j<9;j++){
             temp[38*j+i]=x[9*i+j];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave3(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/3*2*sizeof(int));
     
     for (i=0;i<6;i++){
         for (j=0;j<57;j++){
             temp[57*i+j]=x[6*j+i];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave3(double *x){
     int i,j;
     double *temp;
     temp=(double *) malloc((k+m)/3*2*sizeof(double));
     
     for (i=0;i<6;i++){
         for (j=0;j<57;j++){
             temp[6*j+i]=x[57*i+j];
             }
         }
     
     for (i=0;i<342;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
