#include <stdarg.h>
#include "v.h"

#define PRINTBUFLEN (1024*128)

struct {
  char * out_str;
  mol * inp_mols;
  vibr_t inp_vib;
  int n_inp_mols;
} globals;

char * main_wrap_out(int argc, char * argv[], int * ret) {
  globals.out_str = calloc(PRINTBUFLEN, 1);
  if(!globals.out_str){
    *ret = -1;
    return NULL;
  }
  *ret = main(argc, argv);
  return globals.out_str;
}

int main_wrap_in(int argc, char * argv[], int n_inp_mols, mol * inp_mols, vibr_t inp_vib) {
  globals.inp_mols = inp_mols;
  globals.n_inp_mols = n_inp_mols;
  globals.inp_vib = inp_vib;
  int ret = main(argc, argv);
  globals.inp_mols = NULL;
  globals.n_inp_mols = 0;
  return ret;
}

char * main_wrap_in_out(int argc, char * argv[],
                        int n_inp_mols, mol * inp_mols,
                        vibr_t inp_vib,
                        int * ret){
  globals.out_str = calloc(PRINTBUFLEN, 1);
  if(!globals.out_str){
    *ret = -1;
    return NULL;
  }
  *ret = main_wrap_in(argc, argv, n_inp_mols, inp_mols, inp_vib);
  return globals.out_str;
}

void free_out_str(void){
  FREE0(globals.out_str);
  PRINTOUT(NULL, NULL);
}

void PRINTOUT(FILE * f, char * format, ...){

  va_list args;
  static size_t n = 0;
  static size_t N = PRINTBUFLEN;

  // call to reset n and N
  if(!format){
    n = 0;
    N = PRINTBUFLEN;
    return;
  }

  if(!globals.out_str){
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
  }
  else{

    va_start(args, format);
    size_t size = N-n;
    size_t m = vsnprintf(globals.out_str+n, size, format, args);
    va_end(args);

    if(m >= size){
      N = m < N ? N * 2 : N + 2*m;
      char * tmp = realloc(globals.out_str, N);
      if(!tmp){
        PRINT_ERR("cannot reallocate output buffer\n");
        abort();
      }
      globals.out_str = tmp;
      va_start(args, format);
      vsnprintf(globals.out_str+n, N-n, format, args);
      va_end(args);
    }

    n += m;
  }
}

object * READ_FILES(allpars * ap){
  object * ret;
  if(!globals.inp_mols){
    ret = read_files(ap);
  }
  else{
    ret = acs_from_var(globals.n_inp_mols, globals.inp_mols, globals.inp_vib, ap);
  }
  FREE0(ap->ip.input_files);
  return ret;
}

int SHOULD_PRINT_MAN(int argc){
  if((argc==1) && (!globals.inp_mols)){
    return 1;
  }
  return 0;
}
