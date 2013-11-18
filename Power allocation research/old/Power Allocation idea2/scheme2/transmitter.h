void encode(int *u,int *v){//also generate random data
     int i,j,temp1,temp2;
    
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
  
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }
     v[k+0]=v[0]^v[3];
     v[k+1]=v[2]^v[5]^v[6]^v[8];
     v[k+2]=v[1]^v[4];
     v[k+3]=v[0]^v[6]^v[7]^v[9];
     v[k+4]=v[0]^v[2];
     v[k+5]=v[3]^v[5]^v[8]^v[9];
     v[k+6]=v[1]^v[4];
     v[k+7]=v[2]^v[6]^v[8]^v[9];
     v[k+8]=v[6]^v[7];
     v[k+9]=v[1]^v[4]^v[5]^v[7];
     
}
     
