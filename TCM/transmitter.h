void *input(int *,int *);

void *input(int *u,int *v){
    int i,j,temp;
    int u1[(k+m)/2],u2[(k+m)/2],v1[(k+m)/2],v2[(k+m)/2],v3[(k+m)/2];
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }

       
    for (i=-m;i<(k+m)/2;i++){
        u1[i]=u[i*2];
        u2[i]=u[i*2+1];
        }
    
    
    for (i=0;i<(k+m)/2;i++){                                   //encoding
         v1[i]  = u1[i];
         v2[i]  = u2[i]^u2[i-2]^u2[i-3]^u2[i-5]^u2[i-6];
         v3[i]  = u2[i]^u2[i-1]^u2[i-2]^u2[i-3]^u2[i-6];
        }
   
 
    interleave(v1);
    interleave(v2);
    interleave(v3);
    
    
    for (i=0;i<(k+m)/2;i++){                                   //encoding
        v[3*i]  =v1[i];
        v[3*i+1]=v2[i];
        v[3*i+2]=v3[i];
        }

    }
