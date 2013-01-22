void *input(int *,int *);

void *input(int *u,int *v){
    int i;
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }
    
  
    for (i=0;i<(k+m);i++){                                   //encoding
        
         v[2*i]    = u[i]^u[i-1]^u[i-2];
         v[(2*i)+1]= u[i]^u[i-2];
        
        }
    
   
    }
