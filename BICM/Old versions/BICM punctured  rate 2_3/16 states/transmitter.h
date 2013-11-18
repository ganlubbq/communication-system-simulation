void *input(int *,int *,int);

void *input(int *x,int *z,int index){
    int i;
    int temp[(k+m)*2];
    int v1[(k+m)/2],v2[(k+m)*2/4],v3[(k+m)*2/4];
    
    if(index==0){
       for (i=-m;i<(k+m);i++){
           if (-1<i && i<k) x[i]=rand()%2;
           else x[i]=0;
           }
      }
    
    else{
        for (i=-m;i<0;i++) x[i]=0;
        }
    
    for (i=0;i<(k+m);i++){                                   //encoding
         temp[2*i]    = x[i]^x[i-1]^x[i-2]^x[i-4];
         temp[(2*i)+1]= x[i]^x[i-3]^x[i-4];
        }
    
    
    for (i=0;i<(k+m)/2;i++){
        v1[i]=temp[4*i]  ;
        v2[i]=temp[4*i+1];
        v3[i]=temp[4*i+2];
        }
  
    interleave1(v1);
    interleave2(v2);
    interleave3(v3);
    
    for (i=0;i<(k+m)/2;i++){                                   //encoding
        z[3*i]  =v1[i];
        z[3*i+1]=v2[i];
        z[3*i+2]=v3[i];
        }
 
    
    }
