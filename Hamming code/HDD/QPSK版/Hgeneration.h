int H[N][m];

void generateH(){
     int i,j,count,temp;
     
     //All zero initialization
     for (i=0;i<N;i++){
         for (j=0;j<m;j++){
             H[i][j]=0;
         }
     }
     
     //First row initialization
     H[0][0]=1;
     H[0][1]=1;
     count=2;
     
     for (i=0;i<N-m-1;i++){
         
         //Duplicate the current row
         for (j=0;j<m;j++){
             H[i+1][j]=H[i][j];
         }
         
         //Check for the next combination idx C(m,count)
         temp=0;
         for (j=m-1;j>=m-count;j--){
             temp+=H[i][j];
         }
              
         if (temp==count){
            count++;
            for (j=0;j<count;j++){
                H[i+1][j]=1;
            }
            for (;j<m;j++){
                H[i+1][j]=0;
            }
         }
         
         //Check if the last "1" should step ahead
         else if (H[i][m-1]==0){
                 for (j=m-2;j>=0;j--){
                     if (H[i][j]==1) { break; }
                 }
                 H[i+1][j]=0;
                 H[i+1][j+1]=1;
         }
         
         //Mmm... condition like "0110011" -> "0101110"
         else{
              j=m-2;
              for (;j>=0;j--){
                  if (H[i][j]==0) { break; }
              }
              temp=m-j;
              for (;j>=0;j--){
                  if (H[i][j]==1) { break; }
              }
              H[i+1][j]=0; 
             
              j++;
              for (;temp>0;j++){
                  H[i+1][j]=1;
                  temp--;
              }
              
              for (;j<m;j++){
                  H[i+1][j]=0;
              }
         }
         
     }
     
     //Identity part
     for (i=N-m;i<N;i++){
         H[i][i-(N-m)]=1;
     }

}
