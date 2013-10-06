void input(int *d, float *z){
    int i,j,l,t0,t1;
    int temp[k];
    int a[k+m+m]={0};
    int b[k+m+m]={0};

    for (i=0;i<k;i++){//generate random bits            
        d[i]=rand()&1;
        temp[i]=d[i];
        }
    
    for (i=0;i<k;i++){//encoding a
        for (j=1;j<m+1;j++){
            a[i+m]^= a[i-j+m]*g11[j];
        }
        a[i+m]^= d[i];
    }
   
    for (i=k;i<k+m;i++){//add tail bits so that the 1st shift register goes to 0 state
        d[i]=0;
        for (j=1;j<m+1;j++){
            d[i]=d[i]^a[i-j+m]*g11[j];
        }
    }

    interleave(temp);  
    
    for (i=0;i<k;i++){//encoding b
        for (j=1;j<m+1;j++){
            b[i+m]^= b[i-j+m]*g11[j];
        }
        b[i+m]^= temp[i];
    }

    for (i=0;i<(k+m);i++){//encoding
        t0= 0;
        t1= 0;
        
        for (j=0;j<m+1;j++){
            t0^=a[i-j+m]*g12[j];
            t1^=b[i-j+m]*g12[j];
        }
        
        z[3*i]  = 2.0*d[i] - 1.0;
        z[3*i+1]= 2.0*t0 - 1.0;
        z[3*i+2]= 2.0*t1 - 1.0;
    }

}
    
    
