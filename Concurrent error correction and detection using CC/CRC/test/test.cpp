#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "crc.h"

int main(){
    int i;
    int input[10]={1,1,0,1,0,1,1,0,1,1};
    int fcs[4]={};
    
    crc(input,fcs);
    
    for(i=0;i<4;i++){
    printf("%d ",fcs[i]);
    }
    printf("\n");
    system("pause");
    
    
    crc(input,fcs);
    
    for(i=0;i<4;i++){
    printf("%d ",fcs[i]);
    }
    printf("\n");
    system("pause");
    
    }
