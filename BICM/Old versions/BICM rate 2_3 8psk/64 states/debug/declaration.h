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

int *u,*u1,*u2,*v,*v1,*v2,*v3,*y;

void declaration(void);

void declaration(){
     
     u=(int *) malloc((k+m)*sizeof(int));
    u1=(int *) malloc((k+m)/2*sizeof(int));
    u2=(int *) malloc((k+m)/2*sizeof(int));
     v=(int *) malloc((k+m)/2*3*sizeof(int));
    v1=(int *) malloc(((k+m))/2*sizeof(int));
    v2=(int *) malloc(((k+m))/2*sizeof(int));
    v3=(int *) malloc(((k+m))/2*sizeof(int));
     y=(int *) malloc((k+m)*sizeof(int));
     
     }
