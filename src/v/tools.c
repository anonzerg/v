#include "v.h"
#include "sym.h"

void obj_free(object * ent){
  if(ent->vib){
    free(ent->vib);
  }
  for(int i=0; i<ent->n; i++){
    free(ent->m[i]);
  }
  free(ent->m);
  free(ent);
  return;
}

void pg(atcoord * a, double symtol){

  int n = a->n;
  mol m = {.n = n, .q = a->q, .r=malloc(sizeof(double)*n*3), .name=NULL};
  veccp  (n*3,  m.r, a->r0);
  vecscal(n*3,  m.r, AB);

  molsym * ms = pointgroup(&m, symtol*AB);
  snprintf(a->sym, sizeof(styp), "%s", ms->s);

  free(m.r);
  free(ms);
  return;
}
