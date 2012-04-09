int GF[(1<<m)][m];//m-Tuple representation of GF, Power_of_Alpha = row_index + 1

//Work for m = 1~10, Ex : 1+X+X^4 => 11001
int primitive_poly[11][11]={
0,0,0,0,0,0,0,0,0,0,0,//m = 0
0,0,0,0,0,0,0,0,0,0,0,
1,1,1,0,0,0,0,0,0,0,0,
1,1,0,1,0,0,0,0,0,0,0,
1,1,0,0,1,0,0,0,0,0,0,
1,0,1,0,0,1,0,0,0,0,0,
1,1,0,0,0,0,1,0,0,0,0,
1,0,0,0,1,0,0,1,0,0,0,
1,0,1,1,1,0,0,0,1,0,0,
1,0,0,0,1,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,1 //m = 10
};

void generateGF(){
     int i,j;
   
     //Initialize the 1st and 2nd row
     for (i=0;i<m;i++){
         GF[0][i]=0;
         GF[1][i]=0;
     }
     GF[1][0]=1;
     
     //Construction by right shift and primitive_poly add
     for (i=2;i<(1<<m);i++){
         GF[i][0]=0;
         for (j=1;j<m;j++){
             GF[i][j]=GF[i-1][j-1];
         }  

         if (GF[i-1][m-1] == 1){
            for (j=0;j<m;j++){
                GF[i][j]^=primitive_poly[m][j];
            }
         }
     }
     
     
     //check
     for (i=0;i<(1<<m);i++){
         if (i==0) printf("0 : ");
         else if (i==1) printf("1 : ");
         else if (i>1) printf("a^%d : ",i-1);
         for (j=0;j<m;j++){
              printf("%d",GF[i][j]);   
         }
         printf("\n");
     }
     system("pause");
}
