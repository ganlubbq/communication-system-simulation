//a^(p-1) + a^(n-1)
int addGF(int p, int n){
    int i,j,temp;
    int sum[m];
    
    for (i=0;i<m;i++){
        sum[i]=GF[p][i]^GF[n][i];
    }
    
    for (i=0;i<(1<<m);i++){
        temp=0;
        for (j=0;j<m;j++){
            temp+=sum[j]^GF[i][j];
        }
        
        if (temp == 0){
           return i;  
        }
    }
    
    getchar();
}

//a^(p-1) +* a^(n-1)
int mulGF(int p, int n){
    int expSum;
    
    if (p*n == 0) { return 0; }
    
    else{
        expSum = (p-1) + (n-1);
        expSum %= (1<<m) - 1;
    }
    
    return (expSum+1); 
}

//f(a^(n-1)) = a^-(n-1)
int inverseGF(int n){
    if (n == 0){
       printf("Invalid input!\n");
       getchar();
       return 0;
    }
    
    else if (n == 1) { return 1; }
    
    else { return ( (1<<m) - n + 1 ); }
}
