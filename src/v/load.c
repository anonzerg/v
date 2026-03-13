#include <limits.h>
#include "v.h"
#include "vec3.h"
#include "vecn.h"

#define N_MIN 256

static inline void fill_nf(atcoords * acs, int n0){
  for(int j=n0; j<acs->n; j++){
    acs->m[j]->nf[0] = j-n0;
    acs->m[j]->nf[1] = acs->n-n0;
  }
  return;
}

void acs_readmore(FILE * f, int b, int center, int inertia, int bohr, atcoords * acs, const char * fname){

  // needed to reset nf
  int n0 = acs->n;
  // if continue reading from a previously opened file, find the first molecule from it
  if(ftell(f) && acs->n){
    for(int i=1; i<=acs->n; i++){
      int n1 = acs->n-i;
      if(acs->m[n1]->nf[0]==0){
        n0 = n1;
        break;
      }
    }
  }

  atcoord * m;
  format_t format = UNKNOWN_FORMAT;
  while((m = ac3_read(f, b, center, inertia, bohr, fname, &format))!=NULL){
    if(acs->n==acs->Nmem){
      int N = acs->Nmem ? acs->Nmem*2 : N_MIN;
      atcoord ** ms = realloc(acs->m, N*sizeof(atcoord *));
      if(!ms){
        acs_free(acs);
        free(m);
        PRINT_ERR("cannot reallocate memory\n");
        abort();
      }
      acs->m = ms;
      acs->Nmem = N;
    }
    acs->m[acs->n++] = m;
  }
  fill_nf(acs, n0);
  return;
}

static vibrstr * mode_read_try(FILE * f, atcoord * ac){

  long pos = ftell(f);
  rewind(f);

  int n = ac->n;
  modestr * modes = mode_read(f, n);

  if(modes){
    vibrstr * vib = malloc(sizeof(vibrstr) + sizeof(double)*n*3);
    vib->ac    = ac;
    vib->modes = modes;
    vib->mode0 = (double *)(vib + 1);
    veccp(n*3, vib->mode0, ac->r);
    return vib;
  }
  else{
    fseek(f, pos, SEEK_SET);
    return NULL;
  }
}

static FILE * acs_read_newfile(atcoords * acs, char * fname, drawpars * dp){
  FILE * f;
  if(!strcmp(fname, "-")){
    f = stdin;
  }
  else{
    f = fopen(fname, "r");
    if(!f){
      return NULL;
    }
  }
  acs_readmore(f, dp->b, dp->center, dp->inertia, dp->bohr, acs, fname);
  return f;
}

static void * ent_read(char * fname, drawpars * dp){

  atcoords * acs = malloc(sizeof(atcoords));
  acs->Nmem = 0;
  acs->n = 0;
  acs->m = NULL;

  FILE * f = acs_read_newfile(acs, fname, dp);
  if(!f || !acs->n){
    free(acs);
    return NULL;
  }
  dp->fname = fname;

  if(dp->task==UNKNOWN || dp->task==VIBRO){
    vibrstr * vib = mode_read_try(f, acs->m[acs->n-1]);
    if(vib){
      acs->n--;
      acs_free(acs);
      fclose(f);
      dp->scale = ac3_scale(vib->ac);
      dp->N = vib->modes->n;
      dp->task = VIBRO;
      return vib;
    }
    else{
      if(dp->task==VIBRO){
        PRINT_WARN("the file '%s' does not contain vibrations\n", fname);
      }
    }
  }

  dp->task = AT3COORDS;
  dp->f = f;
  return acs;
}

void * read_files(drawpars * dp){

  int fn = dp->input_files_n;
  char ** flist = dp->input_files;
  void * ent = NULL;
  int i=0;

  // read the first available file
  while(i<fn && !(ent = ent_read(flist[i], dp))){
    PRINT_WARN("cannot read file '%s'\n", flist[i]);
    i++;
  }

  // if the first file does not contain normal modes, try to read other files
  if(ent && (dp->task == AT3COORDS)){
    atcoords * acs = ent;
    int n0 = acs->n;
    for(i++; i<fn; i++){
      FILE * f = acs_read_newfile(acs, flist[i], dp);
      if(!f){
        PRINT_WARN("cannot read file '%s'\n", flist[i]);
      }
      else if(n0==acs->n){
        PRINT_WARN("cannot find molecules in file '%s'\n", flist[i]);
      }
      else{
        fclose(dp->f);
        dp->f = f;
        dp->fname = flist[i];
        n0 = acs->n;
      }
    }
    dp->scale = acs_scale(acs);
    newmol_prep(acs, dp);
    intcoord_check(INT_MAX, dp->z);
  }
  else{
    dp->z[0] = 0;
  }

  return ent;
}

atcoords * acs_from_var(int n, mol * m, drawpars * dp){

  for(int i=0; i<dp->input_files_n; i++){
    PRINT_WARN("ignoring file '%s'\n", dp->input_files[i]);
  }
  if(dp->task==VIBRO){
    PRINT_WARN("cannot read vibrations from input variable\n");
  }
  dp->task = AT3COORDS;

  atcoords * acs = malloc(sizeof(atcoords));
  acs->Nmem = acs->n = n;
  acs->m = malloc(acs->Nmem*sizeof(atcoord *));

  for(int i=0; i<n; i++){
    acs->m[i] = atcoord_fill(m+i, dp->b, dp->center, dp->inertia, dp->bohr);
  }

  fill_nf(acs, 0);
  dp->scale = acs_scale(acs);
  newmol_prep(acs, dp);

  int natmax = 0;
  for(int i=0; i<n; i++){
    natmax = MAX(natmax, m[i].n);
  }
  intcoord_check(natmax, dp->z);

  return acs;
}
