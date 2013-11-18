void encode(int *u,int *v){//also generate random data
     int i,j,temp1,temp2;
    
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
  
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }
     
     for (i=0;i<h;i++){
         v[k+i]=v[i]^v[i+2];
     }
     /*v[k+0]=v[0]^v[2];
     v[k+1]=v[1]^v[3];
     v[k+2]=v[2]^v[4];
     v[k+3]=v[3]^v[5];*/
}
     
