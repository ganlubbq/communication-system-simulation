void input(int *d,int *co){
    int i;
    int a[(k+m)];
    int test[(k+m)];

    for (i=0;i<k;i++){                                  //generate random bits            
        d[i]=rand()%2;
        }
 
    a[0]= d[0];
    a[1]= d[1]^a[0];
    a[2]= d[2]^a[1]^a[0];
    a[3]= d[3]^a[2]^a[1];
    
    //a[-4]=0;a[-3]=0;a[-2]=0;a[-1]=0;
    for (i=4;i<k+m;i++){                                   //encoding a
        if(i>=k) a[i]=0;
        else a[i]= d[i]^a[i-1]^a[i-2]^a[i-4];
        }
   
    for (i=k;i<k+m;i++){          
        d[i]=a[i-1]^a[i-2]^a[i-4];
        }

    a[-4]=0;a[-3]=0;a[-2]=0;a[-1]=0;

    for (i=0;i<(k+m);i++){                                   //encoding
         co[2*i]    = d[i];
         co[(2*i)+1]= a[i]^a[i-3]^a[i-4];
        }


    }
