#include <limits.h>
#include "v.h"
#include "vec3.h"
#include "vecn.h"

#define N_MIN 256

static inline void fill_nf(object * acs, int n0){
  for(int j=n0; j<acs->n; j++){
    acs->m[j]->nf[0] = j-n0;
    acs->m[j]->nf[1] = acs->n-n0;
  }
  return;
}

void acs_readmore(readpars read, int b, geompars geom, object * acs){

  // needed to reset nf
  int n0 = acs->n;
  // if continue reading from a previously opened file, find the first molecule from it
  if(ftell(read.f) && acs->n){
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
  while((m = ac3_read(read, b, geom, &format))!=NULL){
    if(acs->n==acs->Nmem){
      int N = acs->Nmem ? acs->Nmem*2 : N_MIN;
      atcoord ** ms = realloc(acs->m, N*sizeof(atcoord *));
      if(!ms){
        obj_free(acs);
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

static object * mode_read_try(FILE * f, object * ent, drawpars * dp){

  long pos = ftell(f);
  rewind(f);
  atcoord * m = ent->m[ent->n-1];
  vibr_t * vib = mode_read(f, m->n);

  if(!vib){
    fseek(f, pos, SEEK_SET);
    return NULL;
  }
  else{
    for(int i=0; i<ent->n-1; i++){
      free(ent->m[i]);
    }
    ent->Nmem = ent->n = 1;
    ent->m    = realloc(ent->m, sizeof(atcoord *));
    ent->m[0] = m;
    ent->vib  = vib;
    dp->rend.scale = ac3_scale(m);
    dp->N = vib->n;
    return ent;
  }
}

static FILE * acs_read_newfile(char * fname, object * acs, drawpars * dp){
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
  acs_readmore((readpars){f, fname}, dp->rend.bonds, dp->geom, acs);
  return f;
}

static object * ent_read(char * fname, drawpars * dp){

  object * acs = malloc(sizeof(object));
  acs->Nmem = 0;
  acs->n = 0;
  acs->m = NULL;
  acs->vib = NULL;

  FILE * f = acs_read_newfile(fname, acs, dp);
  if(!f || !acs->n){
    free(acs);
    return NULL;
  }
  dp->read.fname = fname;

  if(dp->task==UNKNOWN || dp->task==VIBRO){
    object * vib = mode_read_try(f, acs, dp);
    if(vib){
      fclose(f);
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
  dp->read.f = f;
  return acs;
}

object * read_files(allpars * ap){
  drawpars * dp = &ap->dp;
  initpars * ip = &ap->ip;

  int fn = ip->input_files_n;
  char ** flist = ip->input_files;
  object * ent = NULL;
  int i=0;

  // read the first available file
  while(i<fn && !(ent = ent_read(flist[i], dp))){
    PRINT_WARN("cannot read file '%s'\n", flist[i]);
    i++;
  }

  // if the first file does not contain normal modes, try to read other files
  if(ent && (dp->task == AT3COORDS)){
    object * acs = ent;
    int n0 = acs->n;
    for(i++; i<fn; i++){
      FILE * f = acs_read_newfile(flist[i], acs, dp);
      if(!f){
        PRINT_WARN("cannot read file '%s'\n", flist[i]);
      }
      else if(n0==acs->n){
        PRINT_WARN("cannot find molecules in file '%s'\n", flist[i]);
      }
      else{
        fclose(dp->read.f);
        dp->read.f = f;
        dp->read.fname = flist[i];
        n0 = acs->n;
      }
    }
    dp->rend.scale = acs_scale(acs);
    dp->N = acs->n;
    intcoord_check(INT_MAX, dp->anal.intcoord);
  }
  else{
    dp->anal.intcoord[0] = 0;
  }

  return ent;
}

object * acs_from_var(int n, mol * m, vibr_t vib, allpars * ap){
  drawpars * dp = &ap->dp;
  initpars * ip = &ap->ip;

  for(int i=0; i<ip->input_files_n; i++){
    PRINT_WARN("ignoring file '%s'\n", ip->input_files[i]);
  }
  if(dp->task==UNKNOWN){
    dp->task = vib.n ? VIBRO : AT3COORDS;
  }
  else if(!vib.n && dp->task==VIBRO){
    PRINT_WARN("the input does not contain vibrations\n");
    dp->task = AT3COORDS;
  }

  object * ent = malloc(sizeof(object));

  if(dp->task == AT3COORDS){
    ent->Nmem = ent->n = n;
    ent->m = malloc(ent->Nmem*sizeof(atcoord *));
    ent->vib = NULL;

    for(int i=0; i<n; i++){
      ent->m[i] = atcoord_fill(m+i, dp->rend.bonds, dp->geom, NULL);
    }

    fill_nf(ent, 0);
    dp->rend.scale = acs_scale(ent);
    dp->N = ent->n;

    int natmax = 0;
    for(int i=0; i<n; i++){
      natmax = MAX(natmax, m[i].n);
    }
    intcoord_check(natmax, dp->anal.intcoord);
  }
  else{
    ent->Nmem = ent->n = 1;
    ent->m     = malloc(ent->Nmem*sizeof(atcoord *));
    ent->m[0] = atcoord_fill(m+n-1, dp->rend.bonds, dp->geom, NULL);
    int nat = ent->m[0]->n;
    ent->vib = make_vibr_t(vib.n, nat);
    veccp(vib.n*nat*3, ent->vib->disp, vib.disp);
    veccp(vib.n,       ent->vib->freq, vib.freq);
    veccp(vib.n,       ent->vib->ints, vib.ints);
    veccp(vib.n,       ent->vib->mass, vib.mass);
    dp->rend.scale = ac3_scale(ent->m[0]);
    dp->N = ent->vib->n;
    dp->anal.intcoord[0] = 0;
    dp->read.fname = ent->m[0]->fname;
  }

  return ent;
}
