//h*(q-p)=p*k
#define p 5//column sum=p
#define q 5//row sum=q



void exchange_column(int tempH[k][k+h], int a, int b){//exchange the ath and the bth column
     int i,pass;
     
     for (i=0;i<h;i++){
         pass=tempH[i][a];
         tempH[i][a]=tempH[i][b];
         tempH[i][b]=pass;
     }
}


void exchange(int tempH[k][k+h], int a, int b){//exchange the ath and the bth row
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


int rank_check(int H[][k+h]){
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
            if (j==k+h){
               printf("rank fails\n");
               /*for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d ",tempH[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");*/
     
               return 0;
            }
         }
         
         else if (x != i) { exchange(tempH,i,x); }

         for (j=0;j<h;j++){
             if (j!=i && tempH[j][i]==1){
                vector_add(tempH,i,j);
             }
         }
     }
     
     for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d",tempH[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");
     
     return 1;

}


int select(int map[][k+h],int a,int b,int *y){
     int i,j,count0=0,count=0;
     int choice;
     
     //cal the remaining slots in the ath row
     for (j=0;j<k+h;j++){
         if (map[a][j]==1) count0++;
         
     }
     
     //if the number of 1s to go is larger than the available slots, generation fails
     if (q-b > count0) { *y=0;return 0; }//fails
     
     else{
         choice=rand()%count0;

         count=0;
         for (j=0;j<k+h;j++){
             if (map[a][j]==1){
                if (count==choice) { *y=j;return 1; }
                count++;
             }
             
         }
     }
}


void update(int H[][k+h],int map[][k+h],int x,int y){
     int i,j,m;
     int temp;
     
     map[x][y]=0;
     
     //update column 
     temp=0;
     for (i=0;i<h;i++){
         temp+=H[i][y];
     }
     
     if (temp==p){
        for (i=0;i<h;i++){
            map[i][y]=0;
        }
     }
     
     //update row
     temp=0;
     for (j=0;j<k+h;j++){
         temp+=H[x][j];
     }
     
     if (temp==q){
        for (j=0;j<k+h;j++){
            map[x][j]=0;
        }
     }
     
     //update map according to validity
     for (i=0;i<x;i++){//run row
         temp=0;
         for (j=0;j<k+h;j++){
             temp+=H[i][j]*H[x][j];
         }
         if (temp == 1){
            for (m=0;m<k+h;m++){
                if (H[i][m] == 1) { map[x][m]=0; }
            }
         }
     }
}

void generate_H(int H[h][k+h]){
     int i,j,m,count0=1,count=0,temp,x,y;
     int map[h][k+h],state;
     
     A:
     
     printf("Try %4d\n",count0);count0++;
     
     //initialize H to all zero, map to all 1
     for (i=0;i<h;i++){
         for (j=0;j<k+h;j++){
             H[i][j]=0;
             map[i][j]=1;
         }
     }

     /*//initialize the 1st submatrix
     for (i=0;i<(h/p);i++){
         for (j=0;j<q;j++){
             H[i][i*q+j]=1;
             //map[i][i*q+j]=0;
         }
     }
     
     //initialize map of the 1st submatrix
     for (i=0;i<h/p;i++){
         for (j=0;j<k+h;j++){
             map[i][j]=0;
         }
     }*/

     count=0;
     for (i=0;i<h;i++){//run through the rows from the end of 1st submatrix
         for (j=0;j<q;j++){//number of 1s in a row
             state=select(map,i,j,&y);//randomly choose a column index y
             if (state == 0) { goto A; }//H generation fails, starts from the beginning
             H[i][y]=1;
             update(H,map,i,y);//update the map after setting H[i][y]
             count++;
         }
         
     }           
     
     if (rank_check(H)==0) { goto A; }
           
     printf("count = %d ",count);
     system("pause");      

     //check for row sum
     for (i=0;i<h;i++){
         temp=0;
         for (j=0;j<k+h;j++){
             temp+=H[i][j];
         }
         if (temp != q){
                printf("row sum fails at %d\n",i);
                system("pause");
                //goto B;
         }
     }
     
     //check for column sum
     for (i=0;i<k+h;i++){
         temp=0;
         for (j=0;j<h;j++){
             temp+=H[j][i];
         }
         if (temp != p){
                printf("column sum fails at %d, sum = %d\n",i,temp);
                system("pause");
                //goto B;
         }
     }
     
     //check for "double 1"
     for (i=0;i<k+h-1;i++){
         for (j=i+1;j<k+h;j++){
             temp=0;
             for (m=0;m<h;m++){
                 temp+=H[m][i]*H[m][j];
             }
             if (temp > 1){
                printf("validity check fails");
                system("pause");
                //goto B;
             }
         }
     }
     printf("success");
     system("pause");      
     
     B:
     //print H[][] out
     for (i=0;i<h;i++){
               for (j=0;j<k+h;j++){
                   printf("%d",H[i][j]);
                   
               }
               printf("\n");
     }
     system("pause");
}
