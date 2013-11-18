void *input(int *,int *);

void *input(int *u,int *v){
    int i;
    int u1[(k+m)/2],u2[(k+m)/2],v1[(k+m)*2/3],v2[(k+m)*2/3],v3[(k+m)*2/3];
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }
    
    for (i=0;i<(k+m);i++){                                   //encoding
         v[2*i]    = u[i]^u[i-1]^u[i-2]^u[i-4];
         v[(2*i)+1]= u[i]^u[i-3]^u[i-4];
        }
    
    
    for (i=0;i<(k+m)*2/3;i++){
        v1[i]=v[3*i]  ;
        v2[i]=v[3*i+1];
        v3[i]=v[3*i+2];
        }
  
    interleave1(v1);
    interleave2(v2);
    interleave3(v3);
    
    for (i=0;i<(k+m)*2/3;i++){                                   //encoding
        v[3*i]  =v1[i];
        v[3*i+1]=v2[i];
        v[3*i+2]=v3[i];
        }
 
    
    }
