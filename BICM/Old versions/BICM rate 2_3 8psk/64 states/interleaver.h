void interleave1(int *);
void deinterleave1(float *);
void interleave2(int *);
void deinterleave2(float *);
void interleave3(int *);
void deinterleave3(float *);

void interleave1(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/2*sizeof(int));
     
     for (i=0;i<40;i++){
         for (j=0;j<50;j++){
             temp[50*i+j]=x[40*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave1(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)/2*sizeof(float));
     
     for (i=0;i<40;i++){
         for (j=0;j<50;j++){
             temp[40*j+i]=x[50*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave2(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/2*sizeof(int));
     
     for (i=0;i<20;i++){
         for (j=0;j<100;j++){
             temp[100*i+j]=x[20*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave2(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)/2*sizeof(float));
     
     for (i=0;i<20;i++){
         for (j=0;j<100;j++){
             temp[20*j+i]=x[100*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave3(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/2*sizeof(int));
     
     for (i=0;i<10;i++){
         for (j=0;j<200;j++){
             temp[200*i+j]=x[10*j+i];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave3(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)/2*sizeof(float));
     
     for (i=0;i<10;i++){
         for (j=0;j<200;j++){
             temp[10*j+i]=x[200*i+j];
             }
         }
     
     for (i=0;i<2000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
