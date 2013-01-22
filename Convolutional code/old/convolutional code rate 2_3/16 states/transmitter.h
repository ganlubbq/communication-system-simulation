void *input(int *,int *,int *,int *,int *,int *,int *);

void *input(int *u,int *u1,int *u2,int *v,int *v1,int *v2,int *v3){
    int i,j,temp;
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
        }
   
    for (i=-(m/2);i<(k+m)/2;i++){
        u1[i]=u[i*2];
        u2[i]=u[i*2+1];
        }
  
    for (i=0;i<(k+m)/2;i++){                                   //encoding
        v1[i]=u1[i]^u1[i-1]^u1[i-2]^u2[i-1];
        v2[i]=u1[i-2]^u2[i]^u2[i-2];
        v3[i]=u1[i]^u2[i]^u2[i-1]^u2[i-2];
        }
    
    
       for (i=0;i<(k+m)/2;i++){                                   //encoding
        v[3*i]  =v1[i];
        v[3*i+1]=v2[i];
        v[3*i+2]=v3[i];
        }
   
    
    
    
    
    
    
    
    
    }
