#include "v.h"
#include "vecn.h"
#include "vec3.h"

#define END(S,X) ( (S)->X + (X##_size)/sizeof(*((S)->X)) )

atcoord * atcoord_fill(mol * m0, int b, int center, int inertia, int bohr){

  int n = m0->n;

  size_t q_size = sizeof(int   ) * n;
  size_t r_size = sizeof(double) * n*3;
  atcoord * m;
  if(b != -1){
    size_t bond_a_size = sizeof(int   ) * n*BONDS_MAX;
    size_t bond_r_size = sizeof(double) * n*BONDS_MAX;
    size_t size = sizeof(atcoord) + q_size + r_size + bond_a_size + bond_r_size;
    m = malloc(size);
    m->n = n;
    m->r      = (double *) (m + 1);
    m->bond_r = (double *) END(m,r);
    m->q      = (int    *) END(m,bond_r);
    m->bond_a = (int    *) END(m,q);
    m->bond_flag = 0;
    m->bond_rl = 0.0;
  }
  else{
    size_t size = sizeof(atcoord) + q_size + r_size;
    m = malloc(size);
    m->n = n;
    m->r      = (double *) (m + 1);
    m->bond_r = NULL;
    m->q      = (int    *) END(m,r);
    m->bond_a = NULL;
    m->bond_flag = -1;
    m->bond_rl = 0.0;
  }

  memset(m->sym, 0, sizeof(m->sym));

  for(int i=0; i<n; i++){
    m->q[i] = m0->q[i];
    r3cp(m->r+i*3, m0->r+i*3);
  }
  m->fname = m0->name;

  if(bohr){
    vecscal(n*3, m->r, BA);
  }
  if(inertia){
    // should not change m0
    position(&((mol){.n=n, .q=m->q, .r=m->r}), NULL, 1);
  }
  if(center){
    center_mol(n, m->r, center==2 ? m->q : NULL);
  }

  return m;
}

atcoord * ac3_read(FILE * f, int b, int center, int inertia, int bohr, const char * fname, format_t * format){

  mol * m = NULL;

  switch(*format){
    case XYZ:
      if((m=ac3_read_xyz(f))){
        *format = XYZ;
      }
      break;
    case IN:
      if((m=ac3_read_in(f))){
        *format = IN;
      }
      break;
    case OUT:
      if((m=ac3_read_out(f))){
        *format = OUT;
      }
      break;
    default:
      if((m=ac3_read_xyz(f))){
        *format = XYZ;
      }
      if(!m){
        if((m=ac3_read_in(f))){
          *format = IN;
        }
      }
      if(!m){
        if((m=ac3_read_out(f))){
          *format = OUT;
        }
      }
      break;
  }
  if(!m){
    return NULL;
  }
  m->name = fname;

  atcoord * M = atcoord_fill(m, b, center, inertia, bohr);
  free(m);
  return M;
}

