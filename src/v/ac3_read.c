#include "v.h"
#include "vecn.h"
#include "vec3.h"
#include "matrix.h"

#define EPS_INV 1e-15

static void cell_fill(cellpars * cell, const double abc[9]){
  const double * a = abc+0;
  const double * b = abc+3;
  const double * c = abc+6;
  for(int i=0; i<2; i++){
    for(int j=0; j<2; j++){
      for(int k=0; k<2; k++){
        r3sums3(cell->vertices + (i*4+j*2+k)*3, a, i-0.5, b, j-0.5, c, k-0.5);
      }
    }
  }
  double rot_to_lab_basis[9] = {a[0], b[0], c[0],
                                a[1], b[1], c[1],
                                a[2], b[2], c[2]};
  veccp(9,     cell->rot_to_lab_basis, rot_to_lab_basis);
  mx_id(3,     cell->rot_to_cell_basis);
  mx_inv(3, 3, cell->rot_to_cell_basis, rot_to_lab_basis, EPS_INV);
  cell->boundary = CELL;
  return;
}

atcoord * atcoord_fill(mol * m0, const int b, const geompars geom, const double cell[9]){
  int n = m0->n;

  size_t q_size = sizeof(int   ) * n;
  size_t r_size = sizeof(double) * n*3;
  size_t r0_size = sizeof(double) * n*3;
  struct {size_t r_size; size_t a_size;} bonds = {0, 0};
  if(b!=-1){
    bonds.a_size = sizeof(int   ) * n*BONDS_MAX;
    bonds.r_size = sizeof(double) * n*BONDS_MAX;
  }
  size_t size = sizeof(atcoord) + q_size + r_size + r0_size + bonds.a_size + bonds.r_size;
  atcoord * m = calloc(size, 1);

  if(b==-1){
    m->r       = (double *) (m + 1);
    m->r0      = (double *) MEM_END(m,r);
    m->q       = (int    *) MEM_END(m,r0);
  }
  else{
    m->r       = (double *) (m + 1);
    m->r0      = (double *) MEM_END(m,r);
    m->bonds.r = (double *) MEM_END(m,r0);
    m->q       = (int    *) MEM_END(m,bonds.r);
    m->bonds.a = (int    *) MEM_END(m,q);
  }

  m->n = n;
  m->fname = m0->name;
  for(int i=0; i<n; i++){
    m->q[i] = m0->q[i];
  }
  veccp(n*3, m->r, m0->r);

  if(geom.bohr){
    vecscal(n*3, m->r, BA);
  }
  if(geom.inertia){
    // we should not change m0
    position(&((mol){.n=n, .q=m->q, .r=m->r}), NULL, 1);
  }
  if(geom.center){
    center_mol(n, m->r, geom.center==2 ? m->q : NULL);
  }
  veccp(n*3, m->r0, m->r);

  if(geom.boundary == CELL){
    cell_fill(&m->cell, geom.cell);
  }
  else if(geom.boundary == SHELL){
    m->cell.vertices[0] =  geom.shell[0];
    m->cell.vertices[1] =  geom.shell[1];
    m->cell.boundary = SHELL;
  }
  else if(cell!=NULL){
    cell_fill(&m->cell, cell);
  }

  return m;
}

atcoord * ac3_read(readpars read, int b, const geompars geom, format_t * format){

  mol * m = NULL;
  FILE * f = read.f;
  int cell_found = 0;
  double cell[9] = {};

  switch(*format){
    case XYZ:
      if((m=ac3_read_xyz(f, &cell_found, cell))){
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
      if((m=ac3_read_xyz(f, &cell_found, cell))){
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

  atcoord * M = atcoord_fill(m, b, geom, cell_found ? cell : NULL);
  free(m);
  return M;
}

