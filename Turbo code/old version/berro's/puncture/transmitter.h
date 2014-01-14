void input(int *d,int *z1,int *z2,int *z3){
    int i;
    int temp[k];
    int a[k+m];
    int b[k+m];

    for (i=0;i<k;i++){                                  //generate random bits            
        d[i]=rand()%2;
        temp[i]=d[i];
        }
 
    a[0]= d[0];
    a[1]= d[1]^a[0];
    a[2]= d[2]^a[1]^a[0];
    a[3]= d[3]^a[2]^a[1]^a[0];
    for (i=4;i<k+m;i++){                                   //encoding a
        if(i>=k) a[i]=0;
        else a[i]= d[i]^a[i-1]^a[i-2]^a[i-3]^a[i-4];
        }
   
    for (i=k;i<k+m;i++){          
        d[i]=a[i-1]^a[i-2]^a[i-3]^a[i-4];
        }

    interleave(temp);
    
    b[0]= temp[0];
    b[1]= temp[1]^b[0];
    b[2]= temp[2]^b[1]^b[0];
    b[3]= temp[3]^b[2]^b[1]^b[0];
    for (i=4;i<k+m;i++){                                   //encoding b
        if(i>=k) b[i]=0;
        else b[i]= temp[i]^b[i-1]^b[i-2]^b[i-3]^b[i-4];
        }
    
 
    /*a[-4]=0;a[-3]=0;a[-2]=0;a[-1]=0;
    b[-4]=0;b[-3]=0;b[-2]=0;b[-1]=0;*/
    
    
    z1[0]= d[0];
    z2[0]= a[0];
    z3[0]= b[0];
    
    z1[1]= d[1];
    z2[1]= a[1];
    z3[1]= b[1];
    
    z1[2]= d[2];
    z2[2]= a[2];
    z3[2]= b[2];
    
    z1[3]= d[3];
    z2[3]= a[3];
    z3[3]= b[3];
    
    z1[4]= d[4];
    z2[4]= a[4]^a[0];
    z3[4]= b[4]^b[0];
    
    for (i=5;i<(k+m);i++){                                   //encoding
         z1[i]  = d[i];
         z2[i]= a[i]^a[i-4];
         z3[i]= b[i]^b[i-4];
         }

    /*printf("x:\n");
    for (i=0;i<(k+m);i++){
        printf("%d ",z[3*i]);
        }
    printf("\ny1:\n");
    for (i=0;i<(k+m);i++){
        printf("%d ",z[3*i+1]);
        }
    system("pause");*/
    }
