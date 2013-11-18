void *input(int *,int *);

void *input(int *u,int *v){
    int i,j,temp;
    int u1[(k+m)/2],u2[(k+m)/2],v1[(k+m)/2],v2[(k+m)/2],v3[(k+m)/2];
    
    for (i=-m;i<(k+m);i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }
      
   /* for(j=0;j<k+m;j++){
              printf("%d ",u[j]);
              }
        printf("\n");
        system("pause");  */
       
    for (i=-m;i<(k+m)/2;i++){
        u1[i]=u[i*2];
        u2[i]=u[i*2+1];
        }
    /*printf("u1 ");
    for(j=0;j<(k+m)/2;j++){
              printf("%d ",u1[j]);
              }
        printf("\n");
    printf("u2 "); 
    for(j=0;j<(k+m)/2;j++){
              printf("%d ",u2[j]);
              }
        printf("\n");
        system("pause");  */
    
    
    
    for (i=0;i<(k+m)/2;i++){                                   //encoding
         v1[i]  = u1[i];
         v2[i]  = u2[i]^u2[i-2]^u2[i-3]^u2[i-5]^u2[i-6];
         v3[i]  = u2[i]^u2[i-1]^u2[i-2]^u2[i-3]^u2[i-6];
        }
   
   
   /*printf("v1 ");
    for(j=0;j<(k+m)/2;j++){
              printf("%d ",v1[j]);
              }
        printf("\n");
    printf("v2 "); 
    for(j=0;j<(k+m)/2;j++){
              printf("%d ",v2[j]);
              }
        printf("\n");
        
   printf("v3 "); 
    for(j=0;j<(k+m)/2;j++){
              printf("%d ",v3[j]);
              }
        printf("\n");
        system("pause");  */
   
     
   
   
    /*interleave0(v1);
    interleave1(v2);
    interleave2(v3);*/
    
    for (i=0;i<(k+m)/2;i++){                                   //encoding
        v[3*i]  =v1[i];
        v[3*i+1]=v2[i];
        v[3*i+2]=v3[i];
        }
     
     
    /* for(j=0;j<(k+m)/2*3;j++){
              printf("%d ",v[j]);
              }
        printf("\n");
        system("pause"); */
    }
