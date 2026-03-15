#include "v.h"

mol * ac3_read_xyz(FILE * f){

  // count atoms
  int n = 0;
  if(fscanf(f, "%d", &n) != 1){
    return NULL;
  }

  // skip the comment line
  int c = fgetc(f);
  do{
    c = fgetc(f);
    if(c==EOF){
      GOTOHELL;
    }
  } while(c!='\n');

  // fill in
  mol * m = alloc_mol(n);
  char tmp_str[BIGSTRLEN];
  for(int i=0; i<n; i++){
    styp type;
    if (fscanf (f, "%7s%lf%lf%lf%[^\n]",
          type, m->r+3*i, m->r+3*i+1, m->r+3*i+2, tmp_str) < 4) {
      free(m);
      return NULL;
    }
    m->q[i] = get_element(type);
  }

  return m;
}
