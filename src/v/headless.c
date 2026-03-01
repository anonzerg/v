#include "v.h"
#include "evr.h"

int headless(drawpars * dp, void * ent){
    atcoord * ac = ((atcoords *)ent)->m[dp->n];
    if(dp->b>0 && !ac->bond_flag){
      bonds_fill(dp->rl, dp->bmax, ac);
    }

    char * com = dp->com;
    char c;

    while(1){
      if (dp->com[0]!='\0'){
        c = *(com++);
      }
      else{
        c = getc(stdin);
      }
      if ((c == '\0') || (c == EOF)){
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
            styp sym;
            pg(ac, sym, dp->symtol);
            PRINTOUT(stdout, "%s\n", sym);
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
    ent_free(ent, dp);
    return 0;
}
