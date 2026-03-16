#include "v.h"

#define DISPL_COLUMNS 6
#define SUMMARY_COLUMNS 5
#define DISPL_LINES_SKIP 5
#define SUMMARY_LINES_SKIP 4

static inline vibr_t * make_vibr_t(int n_modes, int n_atoms){
  size_t freq_size = sizeof(double) * n_modes;
  size_t ints_size = sizeof(double) * n_modes;
  size_t mass_size = sizeof(double) * n_modes;
  size_t r0_size   = sizeof(double) * n_atoms*3;
  size_t disp_size = sizeof(double) * n_modes*n_atoms*3;
  size_t size = sizeof(vibr_t) + freq_size + r0_size + disp_size + ints_size + mass_size;
  vibr_t * v = malloc(size);
  v->n    = n_modes;
  v->freq = (double *) (v + 1);
  v->disp = (double *) MEM_END(v,freq);
  v->r0   = (double *) MEM_END(v,disp);
  v->ints = (double *) MEM_END(v,r0);
  v->mass = (double *) MEM_END(v,ints);
  return v;
}

static int readb(FILE * f, int i, int Nmax, int N, int na, vibr_t * vib){
  double d;
  char   s[STRLEN];
  int    t,k,j;

  for (j=0; j<DISPL_LINES_SKIP; j++){
    if (!fgets(s, sizeof(s), f)) {
      return -1;
    }
  }

  for (k=0; k<na*3; k++){
    if (fscanf (f, "%d%c%c", &t, s, s+1) != 3) {
      return -1;
    }
    for (j=0; j<N; j++){
      if (fscanf (f, "%lf", &d) != 1) {
        return -1;
      }
      vib->disp[3*na*(i*Nmax+j)+k] = d;
    }
  }
#if 0
  for (j=0; j<N; j++){
    for (k=0; k<na*3; k++){
      printf("%lf\t", vib->d[3*na*(i*Nmax+j)+k]);
    }
    printf("\n");
  }
#endif
  return 0;
}

int summary_read(FILE * f, vibr_t * vib){
  char s[STRLEN];
  for(int i=0; i<vib->n; i++){
    fgets(s, sizeof(s), f);
    for(int j=0; j<SUMMARY_COLUMNS; j++){
      char * s_end;
      char * ts = strtok(j?NULL:s, "|");
      if(!ts){
        return -1;
      }
#define TS_TO_DOUBLE(X)   { X = strtod(ts, &s_end); if(s_end == ts){ return -1; } }
      else if(j==2){
        TS_TO_DOUBLE(vib->freq[i]);
        if(strchr(ts, 'i')){
          vib->freq[i] *= -1;
        }
      }
      else if(j==3){
        TS_TO_DOUBLE(vib->mass[i]);
      }
      else if(j==4){
        TS_TO_DOUBLE(vib->ints[i]);
      }
#undef TS_TO_DOUBLE
    }
  }
  for (int i=0; i<SUMMARY_LINES_SKIP; i++){
    if (!fgets(s, sizeof(s), f)) {
      return -1;
    }
  }
  return 0;
}

vibr_t * mode_read (FILE * f, int na){

#define READ_S_LINE  { if (!fgets(s, sizeof(s), f)) { return NULL; } }
  char s[STRLEN];
  while (1){
    READ_S_LINE;
    if(strstr(s, "Mode | Freq")){
      break;
    }
  }
  READ_S_LINE;

  int  n = 0;
  long pos = ftell(f);
  while(1){
    READ_S_LINE;
    if(strstr(s, "*------*---------")){
      break;
    }
    else {
      n++;
    }
  }
#undef READ_S_LINE
  fseek(f, pos, SEEK_SET);

  vibr_t * vib = make_vibr_t(n, na);

  if(summary_read(f, vib)){
    goto hell;
  }

  int nb1 = n%DISPL_COLUMNS;
  int nb  = n/DISPL_COLUMNS;
  int i;
  for (i=0; i<(nb1 ? nb+1 : nb); i++){
    if (readb(f, i, DISPL_COLUMNS, (i==nb ? nb1 : DISPL_COLUMNS), na, vib)){
      goto hell;
    }
  }
  return vib;

hell:
  free(vib);
  return NULL;
}

