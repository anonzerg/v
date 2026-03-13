#include "v.h"
#include "evr.h"

void run_commands(FILE * f, char * command, drawpars * dp, object * ent){
  char * com = command;
  char c;

  while(1){

    if(command[0]){
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
          pg(ac, dp->symtol);
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
  atcoord * ac = ent->m[dp->n];
  if(dp->b>0 && !ac->bond_flag){
    bonds_fill(dp->rl, dp->bmax, ac);
  }
  run_commands(stdin, dp->com, dp, ent);
  ent_free(ent, dp);
  return 0;
}
