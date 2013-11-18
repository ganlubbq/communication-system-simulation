void interleave0(int *);
void deinterleave0(void); //de metric of 0 and 1
void interleave1(int *);
void deinterleave1(void);
void interleave2(int *);
void deinterleave2(void);

void interleave0(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/3*sizeof(int));
     
     for (i=0;i<30;i++){
         for (j=0;j<89;j++){
             temp[89*i+j]=x[30*j+i];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave0(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     temp1=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<30;i++){
         for (j=0;j<89;j++){
             temp[30*j+i]=bit_metric[0][0][89*i+j];
             temp1[30*j+i]=bit_metric[0][1][89*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         bit_metric[0][0][i]=temp[i];
         bit_metric[0][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
     
void interleave1(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/3*sizeof(int));
     
     for (i=0;i<15;i++){
         for (j=0;j<178;j++){
             temp[178*i+j]=x[15*j+i];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave1(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     temp1=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<15;i++){
         for (j=0;j<178;j++){
             temp[15*j+i]=bit_metric[1][0][178*i+j];
             temp1[15*j+i]=bit_metric[1][1][178*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         bit_metric[1][0][i]=temp[i];
         bit_metric[1][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
     
void interleave2(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)*2/3*sizeof(int));
     
     for (i=0;i<6;i++){
         for (j=0;j<445;j++){
             temp[445*i+j]=x[6*j+i];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave2(){
     int i,j;
     float *temp,*temp1;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     temp1=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<6;i++){
         for (j=0;j<445;j++){
             temp[6*j+i]=bit_metric[2][0][445*i+j];
             temp1[6*j+i]=bit_metric[2][1][445*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         bit_metric[2][0][i]=temp[i];
         bit_metric[2][1][i]=temp1[i];
         }
     
     free(temp);
     free(temp1);
     }
