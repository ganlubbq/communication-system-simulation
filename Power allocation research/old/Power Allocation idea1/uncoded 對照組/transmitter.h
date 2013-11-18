void encode(int *u,int *v){//also generate random data
     int i,j,temp1,temp2;
    
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
  
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }

}
     
