#include "v.h"

void ac3_print(atcoord * ac, double xy0[2], int b){
  PRINTOUT(stdout, "$molecule\ncart\n");
  for(int k=0; k<ac->n; k++){
    PRINTOUT(stdout, "%3d   % lf   % lf   % lf",
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
        PRINTOUT(stdout, "%s%d", j?",":"    k=", k1+1);
      }
    }
    PRINTOUT(stdout, "\n");
  }
  PRINTOUT(stdout, "$end\n");
  return;
}

void ac3_print_xyz(atcoord * ac, double xy0[2]){
  PRINTOUT(stdout, "%d\n\n", ac->n);
  for(int k=0; k<ac->n; k++){
    const char * s = getname(ac->q[k]);
    int ok = s && s[0];
    if(ok){
      PRINTOUT(stdout, " %-3s", s);
    }
    else{
      PRINTOUT(stdout, " %3d", ac->q[k]);
    }
    PRINTOUT(stdout, "   % lf   % lf   % lf\n",
        xy0[0] + ac->r[k*3  ],
        xy0[1] + ac->r[k*3+1],
                 ac->r[k*3+2]);
  }
  return;
}

void ac3_print2fig(atcoord * ac, double xy0[2], int b, double * v){

  int n = ac->n;
  for(int i=0; i<n; i++){
    PRINTOUT(stdout, "atom %3d% 13.7lf% 13.7lf% 13.7lf\n", ac->q[i],
        xy0[0] + ac->r[i*3  ],
        xy0[1] + ac->r[i*3+1],
                 ac->r[i*3+2]);
  }

  if(v){
    for(int i=0; i<8; i++){
      PRINTOUT(stdout, "atom %3d% 13.7lf% 13.7lf% 13.7lf\n", 0,
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
          PRINTOUT(stdout, "bond %3d %3d\n", k1+1, k+1);
        }
      }
    }
  }

  if(v){
#define LINE(I,J)   PRINTOUT(stdout, "bond %3d %3d % 3d\n", (J)+n+1, (I)+n+1, -1)
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

