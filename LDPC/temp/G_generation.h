void exchange_column(int tempH[k][k+h], int a, int b){//exchange the ath and the bth column
     int i,pass;
     
     for (i=0;i<h;i++){
         pass=tempH[i][a];
         tempH[i][a]=tempH[i][b];
         tempH[i][b]=pass;
     }
}


void exchange_row(int tempH[k][k+h], int a, int b){//exchange the ath and the bth row
     int i,pass;
     
     for (i=0;i<k+h;i++){
         pass=tempH[a][i];
         tempH[a][i]=tempH[b][i];
         tempH[b][i]=pass;
     }
     
}

void vector_add(int tempH[k][k+h], int a, int b){//add the ath row to the bth row
     int i;
     
     for (i=0;i<k+h;i++){
         tempH[b][i]=tempH[b][i]+tempH[a][i] & 1;
     }
}




//derive G from H by gaussian elimination
int get_G(int H[h][k+h],int G[k][k+h]){
     int i,j;
     int tempH[h][k+h];
     int x,y;
     
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             tempH[i][j]=H[i][j];
         }
     }
     
     for (i=0;i<h;i++){
         x=-1;
         for (j=i;j<h;j++){
             if (tempH[j][i]==1){
                x=j;
                break;
             }
         }
         
         if (x == -1) {
            for (j=i+1;j<k+h;j++){
                if (tempH[i][j]==1){
                   exchange_column(tempH,i,j);
                   exchange_column(H,i,j);
                   break;
                }
            }
            if (j==k+h) { printf("error");system("pause"); }
         }
         
         else if (x != i) { exchange_row(tempH,i,x); }

         for (j=0;j<h;j++){
             if (j!=i && tempH[j][i]==1){
                vector_add(tempH,i,j);
             }
         }
     }
     
     
     
     for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d ",tempH[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");
     
     
     
}
