void interleave0(int *);
void deinterleave0(void); //de metric of 0 and 1
void interleave1(int *);
void deinterleave1(void);
void interleave2(int *);
void deinterleave2(void);
void interleave3(int *);
void deinterleave3(void);

void interleave0(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/z*sizeof(int));
     
     for (i=0;i<5;i++){
         for (j=0;j<100;j++){
             temp[100*i+j]=x[5*j+i];
             }
         }
     
     for (i=0;i<500;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave0(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/z*sizeof(float));
     temp1=(float *) malloc((k+m)*2/z*sizeof(float));
     
     for (i=0;i<5;i++){
         for (j=0;j<100;j++){
             temp[5*j+i]=bit_metric[0][0][100*i+j];
             temp1[5*j+i]=bit_metric[0][1][100*i+j];
             }
         }
     
     for (i=0;i<500;i++){
         bit_metric[0][0][i]=temp[i];
         bit_metric[0][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
     
void interleave1(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/z*sizeof(int));
     
     for (i=0;i<20;i++){
         for (j=0;j<25;j++){
             temp[25*i+j]=x[20*j+i];
             }
         }
     
     for (i=0;i<500;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave1(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/z*sizeof(float));
     temp1=(float *) malloc((k+m)*2/z*sizeof(float));
     
     for (i=0;i<20;i++){
         for (j=0;j<25;j++){
             temp[20*j+i]=bit_metric[1][0][25*i+j];
             temp1[20*j+i]=bit_metric[1][1][25*i+j];
             }
         }
     
     for (i=0;i<500;i++){
         bit_metric[1][0][i]=temp[i];
         bit_metric[1][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
     
void interleave2(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/z*sizeof(int));
     
     for (i=0;i<4;i++){
         for (j=0;j<125;j++){
             temp[125*i+j]=x[4*j+i];
             }
         }
     
     for (i=0;i<500;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave2(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/z*sizeof(float));
     temp1=(float *) malloc((k+m)*2/z*sizeof(float));
     
     for (i=0;i<4;i++){
         for (j=0;j<125;j++){
             temp[4*j+i]=bit_metric[2][0][125*i+j];
             temp1[4*j+i]=bit_metric[2][1][125*i+j];
             }
         }
     
     for (i=0;i<500;i++){
         bit_metric[2][0][i]=temp[i];
         bit_metric[2][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }

void interleave3(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/z*sizeof(int));
     
     for (i=0;i<10;i++){
         for (j=0;j<50;j++){
             temp[50*i+j]=x[10*j+i];
             }
         }
     
     for (i=0;i<500;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave3(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/z*sizeof(float));
     temp1=(float *) malloc((k+m)*2/z*sizeof(float));
     
     for (i=0;i<10;i++){
         for (j=0;j<50;j++){
             temp[10*j+i]=bit_metric[3][0][50*i+j];
             temp1[10*j+i]=bit_metric[3][1][50*i+j];
             }
         }
     
     for (i=0;i<500;i++){
         bit_metric[3][0][i]=temp[i];
         bit_metric[3][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
