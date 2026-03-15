#include "v.h"
#include "vecn.h"
#include "vec3.h"

atcoord * atcoord_fill(mol * m0, int b, const geompars geom){
  int n = m0->n;

  size_t q_size = sizeof(int   ) * n;
  size_t r_size = sizeof(double) * n*3;
  struct {size_t r_size; size_t a_size;} bonds = {0, 0};
  if(b!=-1){
    bonds.a_size = sizeof(int   ) * n*BONDS_MAX;
    bonds.r_size = sizeof(double) * n*BONDS_MAX;
  }
  size_t size = sizeof(atcoord) + q_size + r_size + bonds.a_size + bonds.r_size;
  atcoord * m = calloc(size, 1);

  if(b==-1){
    m->r       = (double *) (m + 1);
    m->q       = (int    *) MEM_END(m,r);
  }
  else{
    m->r       = (double *) (m + 1);
    m->bonds.r = (double *) MEM_END(m,r);
    m->q       = (int    *) MEM_END(m,bonds.r);
    m->bonds.a = (int    *) MEM_END(m,q);
  }

  m->n = n;
  m->fname = m0->name;
  for(int i=0; i<n; i++){
    m->q[i] = m0->q[i];
    r3cp(m->r+i*3, m0->r+i*3);
  }

  if(geom.bohr){
    vecscal(n*3, m->r, BA);
  }
  if(geom.inertia){
    // should not change m0
    position(&((mol){.n=n, .q=m->q, .r=m->r}), NULL, 1);
  }
  if(geom.center){
    center_mol(n, m->r, geom.center==2 ? m->q : NULL);
  }

  return m;
}

atcoord * ac3_read(readpars read, int b, const geompars geom, format_t * format){

  mol * m = NULL;
  FILE * f = read.f;

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
  m->name = read.fname;

  atcoord * M = atcoord_fill(m, b, geom);
  free(m);
  return M;
}

