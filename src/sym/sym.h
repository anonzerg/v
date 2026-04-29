#include "mol.h"
#include "vec3.h"
#include "3d.h"

#define EPS1 1e-15

#define MAXCN   16
#define PISTEPS 65536

typedef enum {INV, SIGMA, CN, SN} elsym;

typedef struct {
  double * r;  // vectors associated with generators
  int    * o;  // orders of generators
  elsym  * e;  // types  of generators
  styp     s;  // point group
  int      a;  // number of atoms
  int      n;  // number of group generators
} molsym;

molsym * pointgroup(mol * m, double eps);

