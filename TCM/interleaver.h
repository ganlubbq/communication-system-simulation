void interleave(int *);
void deinterleave(void);


void interleave(int *x){
     int i,j,*temp;
     temp=(int *) malloc((k+m)/2*sizeof(int));
     
     for (i=0;i<50;i++){
         for (j=0;j<20;j++){
             temp[20*i+j]=x[50*j+i];
             }
         }
     
     for (i=0;i<1000;i++){
         x[i]=temp[i];
         }
     
     free(temp);
     }

void deinterleave(){
     int i,j,h;
     float temp[8][(k+m)/2];
     
     
     for (i=0;i<50;i++){
         for (j=0;j<20;j++){
             
             for (h=0;h<8;h++){
             temp[h][50*j+i]=sym_metric[h][20*i+j];}
             }
         }
     
     for (i=0;i<1000;i++){
         for (h=0;h<8;h++){
         sym_metric[h][i]=temp[h][i];
         sym_metric[h][i]=temp[h][i];}
         }
     
    
     }
     

