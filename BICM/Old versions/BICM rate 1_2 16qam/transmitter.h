void *input(int *,int *);

void *input(int *u,int *v){
    int i;
    int v1[(k+m)/2],v2[(k+m)/2],v3[(k+m)/2],v4[(k+m)/2];
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }
    
    for (i=0;i<(k+m);i++){                                   //encoding
         v[2*i]    = u[i]^u[i-2]^u[i-3]^u[i-5]^u[i-6];
         v[(2*i)+1]= u[i]^u[i-1]^u[i-2]^u[i-3]^u[i-6];
        }
    
    
    for (i=0;i<(k+m)*2/z;i++){
        v1[i]=v[z*i]  ;
        v2[i]=v[z*i+1];
        v3[i]=v[z*i+2];
        v4[i]=v[z*i+3];
        }
   
    interleave1(v1);
    interleave2(v2);
    interleave3(v3);
    interleave4(v4);
    
    for (i=0;i<(k+m)*2/z;i++){                                   //encoding
        v[z*i]  =v1[i];
        v[z*i+1]=v2[i];
        v[z*i+2]=v3[i];
        v[z*i+3]=v4[i];
        }
     

    
    }
