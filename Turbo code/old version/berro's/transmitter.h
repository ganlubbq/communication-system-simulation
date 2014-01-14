void input(int *d,int *z){
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
    
    
    z[0]= d[0];
    z[1]= a[0];
    z[2]= b[0];
    
    z[3]= d[1];
    z[4]= a[1];
    z[5]= b[1];
    
    z[6]= d[2];
    z[7]= a[2];
    z[8]= b[2];
    
    z[9] = d[3];
    z[10]= a[3];
    z[11]= b[3];
    
    z[12]= d[4];
    z[13]= a[4]^a[0];
    z[14]= b[4]^b[0];
    
    for (i=5;i<(k+m);i++){                                   //encoding
         z[3*i]  = d[i];
         z[3*i+1]= a[i]^a[i-4];
         z[3*i+2]= b[i]^b[i-4];
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
