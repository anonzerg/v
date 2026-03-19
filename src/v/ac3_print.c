#include "v.h"
#include "matrix.h"
#include "vec3.h"

void ac3_print(atcoord * ac, rendpars * rend){
  PRINTOUT(stdout, "$molecule\ncart\n");
  for(int k=0; k<ac->n; k++){
    PRINTOUT(stdout, "%3d   % lf   % lf   % lf",
        ac->q[k],
        rend->xy0[0] + ac->r[k*3  ],
        rend->xy0[1] + ac->r[k*3+1],
                 ac->r[k*3+2]);
    if(rend->bonds>0){
      for(int j=0; j<BONDS_MAX; j++){
        int k1 = ac->bonds.a[k*BONDS_MAX+j];
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

void ac3_print_xyz(atcoord * ac, rendpars * rend, cellpars * cell){
  PRINTOUT(stdout, "%d\n", ac->n);
  if(cell->vert==1){
    double C[9];
    mx_multmx(3,3,3, C, rend->ac3rmx, cell->rot_to_lab_basis);
    PRINTOUT(stdout, "Lattice=\"%lf %lf %lf %lf %lf %lf %lf %lf %lf\"\n",
        C[0], C[3], C[6],
        C[1], C[4], C[7],
        C[2], C[5], C[8]);
  }
  else{
    PRINTOUT(stdout, "\n");
  }

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
        rend->xy0[0] + ac->r[k*3  ],
        rend->xy0[1] + ac->r[k*3+1],
                       ac->r[k*3+2]);
  }
  return;
}

void ac3_print2fig(atcoord * ac, rendpars * rend, cellpars * cell){
  int n = ac->n;
  for(int i=0; i<n; i++){
    PRINTOUT(stdout, "atom %3d% 13.7lf% 13.7lf% 13.7lf\n", ac->q[i],
        rend->xy0[0] + ac->r[i*3  ],
        rend->xy0[1] + ac->r[i*3+1],
                 ac->r[i*3+2]);
  }

  if(cell->vert==1){
    for(int i=0; i<8; i++){
      double v[3];
      r3mx(v, cell->vertices+3*i, rend->ac3rmx);
      PRINTOUT(stdout, "atom %3d% 13.7lf% 13.7lf% 13.7lf\n", 0,
          rend->xy0[0] + v[0], rend->xy0[1] + v[1], v[2]);
    }
  }

  if(rend->bonds>0){
    for(int k=0; k<n; k++){
      for(int j=0; j<BONDS_MAX; j++){
        int k1 = ac->bonds.a[k*BONDS_MAX+j];
        if(k1 == -1 ){
          break;
        }
        if(k1 < k){
          PRINTOUT(stdout, "bond %3d %3d\n", k1+1, k+1);
        }
      }
    }
  }

  if(cell->vert){
#define LINE(I,J)   PRINTOUT(stdout, "bond %3d %3d % 3d\n", (J)+n+1, (I)+n+1, -1)
    for(int i=0; i<8; i+=2){
      LINE(i,i+1); // || z-axis
    }
    for(int j=0; j<2; j++){
      for(int i=0; i<2; i++){
        LINE(i*4+j, i*4+2+j);  // || y-axis
        LINE(i*2+j, i*2+4+j);  // || x-axis
      }
    }
#undef LINE
  }

  return;
}

