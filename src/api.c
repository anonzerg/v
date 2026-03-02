#include <stdarg.h>
#include "v.h"

#define PRINTBUFLEN (1024*128)
#define FREE0(PTR) { free(PTR); PTR = NULL; }

char * out_str;
in_str_t in_str;

int main_wrapper1 (int argc, char * argv[]) {
  out_str = calloc(PRINTBUFLEN, 1);
  int ret = main(argc, argv);
  FREE0(out_str);
  return ret;
}

char * main_wrapper2 (int argc, char * argv[]) {
  out_str = calloc(PRINTBUFLEN, 1);
  main(argc, argv);
  return out_str;
}

char * main_wrapper3 (in_str_t in_str_, int argc, char * argv[]) {
  in_str = in_str_;
  out_str = calloc(PRINTBUFLEN, 1);
  main(argc, argv);
  return out_str;
}

void free_out_str(void){
  FREE0(out_str);
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

  if(!out_str){
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
  }
  else{

    va_start(args, format);
    size_t size = N-n;
    size_t m = vsnprintf(out_str+n, size, format, args);
    va_end(args);

    if(m >= size){
      N = m < N ? N * 2 : N + 2*m;
      out_str = realloc(out_str, N);
      va_start(args, format);
      vsnprintf(out_str+n, N-n, format, args);
      va_end(args);
    }

    n += m;
  }
}

void * READ_FILES(drawpars * dp){
  void * ret;
  if(!in_str.n){
    ret = read_files(dp);
  }
  else{
    ret = get_in_str(in_str, dp);
  }
  FREE0(dp->input_files);
  return ret;
}

int SHOULD_PRINT_MAN(int argc){
  if((argc==1) && (!in_str.n)){
    return 1;
  }
  return 0;
}
