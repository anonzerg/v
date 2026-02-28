#include "v.h"

char * out_str;
int main (int argc, char * argv[]);

int main_wrapper1 (int argc, char * argv[]) {
  out_str = calloc(PRINTBUFLEN, 1);
  main(argc, argv);
  free(out_str);
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
