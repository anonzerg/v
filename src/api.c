#include <stdarg.h>
#include "v.h"

#define PRINTBUFLEN (1024*128)

char * out_str;
in_str_t in_str;

int main_wrapper1 (int argc, char * argv[]) {
  out_str = calloc(PRINTBUFLEN, 1);
  int ret = main(argc, argv);
  free(out_str);
  return ret;
}

char * main_wrapper2 (int argc, char * argv[]) {
  out_str = calloc(PRINTBUFLEN, 1);
  main(argc, argv);
  return out_str;
}

void free_out_str(void){
  free(out_str);
  out_str = NULL;
}

void PRINTOUT(FILE * f, char * format, ...){

  va_list args;
  static size_t n = 0;
  static size_t N = PRINTBUFLEN;

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

  ////////////////////////////////////////////
  // this should be set up by a wrapper that calls main
  //in_str.n = 2;
  //int q[] = {1, 1};
  //double r[] = {0, 0, 0, 1, 0, 0};
  //char * const name = "molname";
  //in_str.q = q;
  //in_str.r = r;
  //in_str.name = name;
  ////////////////////////////////////////////

  void * ret;
  if(!in_str.n){
    ret = read_files(dp);
  }
  else{
    ret = get_in_str(in_str, dp);
  }

  free(dp->input_files);
  dp->input_files = NULL;
  return ret;

}
