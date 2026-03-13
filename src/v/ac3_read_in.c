#include <ctype.h>
#include "v.h"
#include "3d.h"
#include "vec3.h"

int read_cart_atom(FILE * f, int n, mol * m){
  int q;
  double r[3];
  int res = (fscanf(f, "%d%lf%lf%lf", &q, r, r+1, r+2)==4);
  if(res && m){
    m->q[n] = q;
    r3cp(m->r+3*n, r);
  }
  return res;
}

static int read_z_atom( FILE * f, int n, mol * m){

  int  res, q, a, b, c;
  double R, phi, theta;

  switch(n){
    case 0:
      res = (fscanf(f, "%d", &q) == 1);
      break;
    case 1:
      res = (fscanf(f, "%d%d%lf", &q, &a, &R) == 3);
      break;
    case 2:
      res = (fscanf(f, "%d%d%lf%d%lf", &q, &a, &R, &b, &phi) == 5);
      break;
    default:
      res = (fscanf(f, "%d%d%lf%d%lf%d%lf", &q, &a, &R, &b, &phi, &c, &theta) == 7);
      break;
  }

  if(res && m){
    m->q[n] = q;
    res = !zmat2cart(n, m->r+3*n,
                     m->r+3*(a-1), m->r+3*(b-1), m->r+3*(c-1),
                     R, phi*DEG2RAD, theta*DEG2RAD);
  }
  return res;
}

static inline int read_atom(FILE * f, int zmat, int n, mol * m){
  if(zmat){
    return read_z_atom(f, n, m);
  }
  else{
    return read_cart_atom(f, n, m);
  }
}

static int read_atoms(FILE * f, int zmat, mol * m){
  char s[STRLEN];
  int n = 0;
  while (fscanf(f, " $%255s", s) != 1){
    if(fscanf(f, "set =%255s", s)==1){
      continue;
    }
    if(!read_atom(f, zmat, n, m)){
      return 0;
    }
    // ignore the rest of the line
    int tc;
    while((tc=getc(f))!='\n'){
      if(tc==EOF){
        return 0;
      }
    }
    n++;
  }
  return n;
}

static inline int strlcmp(const char * const s1, const char * const s2){
  return strncmp(s1, s2, MIN(strlen(s1), strlen(s2)));
}

mol * ac3_read_in(FILE * f){

  int bohr = 0;
  int zmat = 0;
  char s[STRLEN];
  char key[STRLEN];
  char val[STRLEN];
  long pos0 = ftell(f);

  // find where the molecule begins
  while(1){
    memset(s, '\0', sizeof(s));
    if(!fgets(s, sizeof(s), f)){
      goto hell;
    }
    char * s0 = s;
    while(s0[0] && (s0[0]==' ' || s[0]=='\t')){
      s0++;
    }
    if(!strcmp(s0, "$molecule\n")){
      break;
    }
  }

  // read the keyword block up until cart/zmat
  while (fscanf(f, " %255[^$ \n]", s) == 1) {
    for(int i=0; i<strlen(s); i++){
      s[i] = tolower(s[i]);
    }
    if(sscanf(s, "%255[^=]=%255s", key, val)==2){
      if(!strlcmp(key, "units")){
        if(!strlcmp(val, "bohr")){
          bohr = 1;
        }
        else if(!strlcmp(val, "angstrom")){
          bohr = 0;
        }
        else{
          PRINT_WARN("Unknown units in '%s'\n", s);
        }
      }
    }
    else if(! strlcmp(s, "cartesian") ){
      zmat = 0;
      break;
    }
    else if(! strlcmp(s, "z-matrix") ){
      zmat = 1;
      break;
    }
  }

  // count atoms
  long pos1 = ftell(f);
  int n = read_atoms(f, zmat, NULL);
  if(!n){
    goto hell;
  }
  fseek(f, pos1, SEEK_SET);

  // fill in
  mol * m = alloc_mol(n);
  if(!read_atoms(f, zmat, m)){
    // error in zmat conversion
    free(m);
    goto hell;
  }
  if(bohr){
    vecscal(3*n, m->r, BA);
  }
  return m;

hell:
  fseek(f, pos0, SEEK_SET);
  return NULL;
}
