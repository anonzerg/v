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
  mol * m = calloc(sizeof(mol)+3*n*sizeof(double)+n*sizeof(int), 1);
  m->n = n;
  m->r = (double *)(m + 1);
  m->q = (int *)(m->r + 3*n);
  return m;
}

void   position(mol * m, double d[3], int preserve_chirality);
void   center_mol(int n, double * r, int * q);

int    intcoord_check(int n, int z[5]);
double intcoord_calc (int r_units_a, int check_n, int z[5], double * r);

#endif
