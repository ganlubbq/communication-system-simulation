void input(int *u,int *v){
    int i,j,temp1,temp2;
    
    for (i=-m;i<0;i++){
        u[i]=0;
    }
    for (i=0;i<k;i++){
        u[i]=rand()%2;
    }
    for (i=k;i<k+m;i++){
        u[i]=0;
    }
  
    for (i=0;i<k+m;i++){                                   //encoding
        
        temp1=u[i]*g11[0];
        temp2=u[i]*g12[0];
        
        for (j=1;j<=m;j++){
        
            temp1=temp1^u[i-j]*g11[j];
            temp2=temp2^u[i-j]*g12[j];
            }
        
        v[2*i]=temp1;
        v[(2*i)+1]=temp2;
        }
    
   
    }
