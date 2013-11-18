
#define k 6                          //pac length
#define h 4                          //parity bits

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

FILE	*fp_output;
char data[20]={"data.txt"};
    
#include"hd_decoder.h"

int main(){
    int i,j=0,t,temp,temp1;
    int s,min,hour;
    long temptime1,temptime2;
    
    srand(time(&temptime1)%60);//seed
    fp_output=fopen(data,"w");



    bound();
 
 
    time(&temptime2);//calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    

    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    fprintf(fp_output,"你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    printf("你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    fclose(fp_output);
    system("pause");
}
