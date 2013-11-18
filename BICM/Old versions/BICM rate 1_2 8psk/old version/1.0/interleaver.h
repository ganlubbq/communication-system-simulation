void interleave1(int *);
void deinterleave1(float *);
void interleave2(int *);
void deinterleave2(float *);
void interleave3(int *);
void deinterleave3(float *);

void interleave1(int *x){
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

void deinterleave1(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<30;i++){
         for (j=0;j<89;j++){
             temp[30*j+i]=x[89*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave2(int *x){
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

void deinterleave2(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<15;i++){
         for (j=0;j<178;j++){
             temp[15*j+i]=x[178*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
     
void interleave3(int *x){
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

void deinterleave3(float *x){
     int i,j;
     float *temp;
     temp=(float *) malloc((k+m)*2/3*sizeof(float));
     
     for (i=0;i<6;i++){
         for (j=0;j<445;j++){
             temp[6*j+i]=x[445*i+j];
             }
         }
     
     for (i=0;i<2670;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }
