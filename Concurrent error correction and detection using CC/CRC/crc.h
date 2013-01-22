void crc(int *input){
     int i,j;
     int reg[r]={},temp;

     for(i=0;i<k;i++){
         temp=reg[0];             
         for(j=0;j<r-1;j++){
            reg[j]=reg[j+1]^(temp*gcrc[j+1]);
            }
         reg[r-1]=input[i]^(temp*gcrc[r]);
         }
     
     for(i=k-r;i<k;i++){
         input[i]=reg[i-(k-r)];
         }
     
     }
