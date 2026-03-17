#include "v.h"
#include "evr.h"

void run_commands(FILE * f, char * command, drawpars * dp, object * ent){
  char * com = command;
  char c;

  while(1){

    if(command && command[0]){
      c = *(com++);
    }
    else if(f){
      c = getc(f);
    }
    else{
      c = 0;
    }
    if((!c) || (c == EOF)){
      break;
    }

    switch(c){
      case('p'):
        kp_print2fig(ent, dp); break;
      case('z'):
        kp_print_xyz(ent, dp); break;
      case('x'):
        kp_print(ent, dp); break;
      case('.'):
        {
          atcoord * ac = ent->m[dp->n];
          pg(ac, dp->anal.symtol);
          PRINTOUT(stdout, "%s\n", ac->sym);
        }; break;

      case(' '):
      case('\n'):
        break;

      default:
        {
          PRINT_WARN("Unknown command: %c\n", c); break;
        }
    }
  }
  return;
}

int headless(drawpars * dp, object * ent){
  fill_bonds(ent->m[dp->n], dp);
  rotate_mol(ent->m[dp->n], dp);
  run_commands(stdin, dp->ui.com, dp, ent);
  obj_free(ent);
  CLOSE0(dp->read.f);
  return 0;
}
