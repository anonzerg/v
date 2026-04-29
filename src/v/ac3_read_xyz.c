#include "v.h"
#include "vecn.h"

static inline int eat_line(FILE * f){
  int c;
  do{
    c = fgetc(f);
    if(c==EOF){
      return -1;
    }
  } while(c!='\n');
  return 0;
}

static int read_header(FILE * f, double cell[9]){

  const char pattern[] = "Lattice=\"";

  int c, i=0, found=0;
  do{
    c = fgetc(f);
    if(c==EOF){
      return -1;
    }
    if(c==pattern[i]){
      if(++i==sizeof(pattern)-1){
        found = 1;
        break;
      }
    }
    else{
      i=0;
    }
  } while(c!='\n');

  if(!found){
    return 0;
  }

  int ret = (fscanf(f, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", cell, cell+1, cell+2, cell+3, cell+4, cell+5, cell+6, cell+7, cell+8)==9);

  if(eat_line(f)){
    return -1;
  }

  return ret;
}

mol * ac3_read_xyz(FILE * f, int * cell_found, double * cell){

  // count atoms
  int n = 0;
  if(fscanf(f, "%d", &n) != 1){
    return NULL;
  }
  fgetc(f);

  // read header
  double tcell[9];
  int r = read_header(f, tcell);
  if(r==-1){
    return NULL;
  }
  else if(r==1){
    veccp(9, cell, tcell);
    *cell_found = 1;
  }

  // fill in
  mol * m = alloc_mol(n);
  for(int i=0; i<n; i++){
    styp type;
    if (fscanf (f, "%7s%lf%lf%lf", type, m->r+3*i, m->r+3*i+1, m->r+3*i+2) != 4){
      free(m);
      return NULL;
    }
    m->q[i] = get_element(type);

    if(eat_line(f)){
      free(m);
      return NULL;
    }
  }

  return m;
}
