int **H;

int GallagerH[15][20]={
1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,

1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,
0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,
0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,
0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,

1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,
0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,
0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,
0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,
0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1
};


void declare(void){
     int i,j;
     
     H=(int **)malloc(h*sizeof(int *));

     for (i=0;i<h;i++){
         *(H+i)=(int *)malloc((k+h)*sizeof(int));
     }

     //initialize H[][] here A-list!?


     //Set H[][]=Gallager[][]
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             H[i][j]=GallagerH[i][j];
         }
     }
     

}
