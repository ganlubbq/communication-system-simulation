void encode(int *u,int *v){//also generate random data
     int i,j,l,temp;
    
     for (i=0;i<k;i++){
         u[i]=rand()&1;
     }
  
     //encode
     for (i=0;i<k;i++){
         v[i]=u[i];
     }
     

         //parity bits
         for (j=0;j<h;j++){
             temp = 0;
             for (l=0;l<k;l++){
                 temp^=(v[l]*Ht[l][j]);
             }
             v[k+j]=temp;
         }
}
     
