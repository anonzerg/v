#include "v.h"

static inline vibr_t * make_vibr_t(int n_modes, int n_atoms){
  size_t freq_size = sizeof(double) * n_modes;
  size_t r0_size   = sizeof(double) * n_atoms*3;
  size_t disp_size = sizeof(double) * n_modes*n_atoms*3;
  size_t size = sizeof(vibr_t) + freq_size + r0_size + disp_size;
  vibr_t * v = malloc(size);
  v->n    = n_modes;
  v->freq = (double *) (v + 1);
  v->disp = (double *) MEM_END(v,freq);
  v->r0   = (double *) MEM_END(v,disp);
  return v;
}

static int readb(FILE * f, int i, int Nmax, int N, int na, vibr_t * m){
  double d;
  char   s[STRLEN];
  int    t,k,j;

  for (j=0; j<3; j++){
    if (!fgets(s, sizeof(s), f)) {
      return -1;
    }
  }

  for (j=0; j<N; j++){
    if (fscanf (f, (j ? "%lf" : " freq. %lf" ), &d) != 1) {
      return -1;
    }
    if (fscanf (f, "%c%c", s, s+1) != 2) {
      return -1;
    }
    if(s[1]=='i'){
      d = -d;
    }
    m->freq[i*Nmax+j] = d;
  }

  if (!fgets(s, sizeof(s), f)) {
    return -1;
  }
  if (!fgets(s, sizeof(s), f)) {
    return -1;
  }

  for (k=0; k<na*3; k++){

    if (fscanf (f, "%d%c%c", &t, s, s+1) != 3) {
      return -1;
    }

    for (j=0; j<N; j++){
      if (fscanf (f, "%lf", &d) != 1) {
        return -1;
      }
      m->disp[3*na*(i*Nmax+j)+k] = d;
    }
  }
#if 0
  for (j=0; j<N; j++){
    for (k=0; k<na*3; k++){
      printf("%lf\t", m->d[3*na*(i*Nmax+j)+k]);
    }
    printf("\n");
  }
#endif
  return 0;
}

vibr_t * mode_read (FILE * f, int na){

  const int N = 6;
  int  n = 0;
  char s[STRLEN];

  while (1){
    if (!fgets(s, sizeof(s), f)) {
      return NULL;
    }
    if(strstr(s, "Mode | Freq")){
      break;
    }
  }
  if (!fgets(s, sizeof(s), f)){
    return NULL;
  }
  while(1){
    if (!fgets(s, sizeof(s), f)) {
      return NULL;
    }
    if(strstr(s, "*------*---------")){
      break;
    }
    else {
      n++;
    }
  }

  vibr_t * m = make_vibr_t(n, na);

  for (int i=0; i<3; i++){
    if (!fgets(s, sizeof(s), f)) {
      goto hell;
    }
  }

  int nb1 = n%N;
  int nb  = n/N;
  int i;
  for (i=0; i<nb; i++){
    if (readb(f, i, N, N, na, m)){
      goto hell;
    }
  }
  if(nb1){
    if (readb(f, i, N, nb1, na, m)){
      goto hell;
    }
  }

  return m;

hell:
  free(m);
  return NULL;
}

