#include "v.h"
#include <stdarg.h>


extern char * out_str;


static void PRINTOUT(char * format, ...){

  va_list args;
  static size_t n = 0;
  static size_t N = PRINTBUFLEN;

  if(!out_str){
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
  }
  else{

    va_start(args, format);
    size_t size = N-n;
    size_t m = vsnprintf(out_str+n, size, format, args);
    va_end(args);

    if(m >= size){
      N = m < N ? N * 2 : N + 2*m;
      out_str = realloc(out_str, N);
      va_start(args, format);
      vsnprintf(out_str+n, N-n, format, args);
      va_end(args);
    }

    n += m;
  }
}




void ac3_print(atcoord * ac, double xy0[2], int b){
  PRINTOUT("$molecule\ncart\n");
  for(int k=0; k<ac->n; k++){
    PRINTOUT("%3d   % lf   % lf   % lf",
        ac->q[k],
        xy0[0] + ac->r[k*3  ],
        xy0[1] + ac->r[k*3+1],
                 ac->r[k*3+2]);
    if(b>0){
      for(int j=0; j<BONDS_MAX; j++){
        int k1 = ac->bond_a[k*BONDS_MAX+j];
        if(k1 == -1 ){
          break;
        }
        PRINTOUT("%s%d", j?",":"    k=", k1+1);
      }
    }
    PRINTOUT("\n");
  }
  PRINTOUT("$end\n");
  return;
}

void ac3_print_xyz(atcoord * ac, double xy0[2]){
  PRINTOUT("%d\n\n", ac->n);
  for(int k=0; k<ac->n; k++){
    const char * s = getname(ac->q[k]);
    int ok = s && s[0];
    if(ok){
      PRINTOUT(" %-3s", s);
    }
    else{
      PRINTOUT(" %3d", ac->q[k]);
    }
    PRINTOUT("   % lf   % lf   % lf\n",
        xy0[0] + ac->r[k*3  ],
        xy0[1] + ac->r[k*3+1],
                 ac->r[k*3+2]);
  }
  return;
}

void ac3_print2fig(atcoord * ac, double xy0[2], int b, double * v){

  int n = ac->n;
  for(int i=0; i<n; i++){
    PRINTOUT("atom %3d% 13.7lf% 13.7lf% 13.7lf\n", ac->q[i],
        xy0[0] + ac->r[i*3  ],
        xy0[1] + ac->r[i*3+1],
                 ac->r[i*3+2]);
  }

  if(v){
    for(int i=0; i<8; i++){
      PRINTOUT("atom %3d% 13.7lf% 13.7lf% 13.7lf\n", 0,
          xy0[0] + v[i*3  ],
          xy0[1] + v[i*3+1],
                   v[i*3+2]);
    }
  }

  if(b>0){
    for(int k=0; k<n; k++){
      for(int j=0; j<BONDS_MAX; j++){
        int k1 = ac->bond_a[k*BONDS_MAX+j];
        if(k1 == -1 ){
          break;
        }
        if(k1 < k){
          PRINTOUT("bond %3d %3d\n", k1+1, k+1);
        }
      }
    }
  }

  if(v){
#define LINE(I,J)   PRINTOUT("bond %3d %3d % 3d\n", (J)+n+1, (I)+n+1, -1)
    LINE(0,1);
    LINE(0,2);
    LINE(0,3);
    LINE(1,4);
    LINE(1,5);
    LINE(2,4);
    LINE(2,6);
    LINE(3,5);
    LINE(3,6);
    LINE(4,7);
    LINE(5,7);
    LINE(6,7);
#undef LINE
  }

  return;
}

