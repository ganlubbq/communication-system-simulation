

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include"gaussian.h"
#include"random.h"

int main(){
    int i,j=0,t,temp,temp1;
    int s,min,hour;
    long temptime1,temptime2;
    float x;
 
    srand(time(&temptime1)%60);//seed                                                
 
 

    for (i=0;i<100000000;i++){
        
        x=float(rand())/RAND_MAX;
        //x=gaurand();
        //x=awgn();
        //x=Normal(0,1);
    }
 
 
 
 
 
    time(&temptime2);//calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;

    
    printf("你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    
    system("pause");
}
