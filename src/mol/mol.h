#ifndef MOL_H
#define MOL_H

#include "common.h"

typedef struct {
  double * r;
  int    * q;
  const char * name;
  int      n;
} mol;

static inline mol * alloc_mol(int n){
  size_t r_size = sizeof(double) * n*3;
  size_t q_size = sizeof(int   ) * n;
  mol * m = calloc(sizeof(mol)+r_size+q_size, 1);
  m->r = (double *) (m + 1);
  m->q = (int    *) MEM_END(m, r);
  m->name = NULL;
  m->n = n;
  return m;
}

void   position(mol * m, double d[3], int preserve_chirality);
void   center_mol(int n, double * r, const int * q);

int    intcoord_check(int n, int z[5]);
double intcoord_calc (int r_units_a, int check_n, const int z[5], const double * r);

#endif
