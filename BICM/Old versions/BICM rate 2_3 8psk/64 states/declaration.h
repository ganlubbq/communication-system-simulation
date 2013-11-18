struct state{
       struct state *from;
       int in[2];
       //float weight;
       };
            
struct complex{
       float re;
       float im;
       };

struct state node[(k+m)/2+1][st_num];

struct complex mapping[8];

int next_table[64][4];
int out_table[64][12];
float ray[(k+m)/2];

int *u,*v,*y;

void declaration(void);

void declaration(){
     
     u=(int *) malloc((k+m)*sizeof(int));
    
     v=(int *) malloc((k+m)/2*3*sizeof(int));
   
     y=(int *) malloc((k+m)*sizeof(int));
     
     }
