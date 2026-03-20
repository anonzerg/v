#include "v.h"
#include "evr.h"

#define GUI(C) if(!dp->ui.gui){ \
    PRINT_WARN("Ignoring command `%c` in the headless mode\n", C); \
    break;\
  }

static void run_command(char c, drawpars * dp, object * ent){
  switch(c){
    case('>'):
      GUI(c); kp_fw_toggle(ent, dp); break;
    case('f'):
      GUI(c); kp_film(ent, dp); break;
    case('d'):
      GUI(c); kp_move_r(ent, dp); break;
    case('a'):
      GUI(c); kp_move_l(ent, dp); break;
    case('w'):
      GUI(c); kp_move_u(ent, dp); break;
    case('s'):
      GUI(c); kp_move_d(ent, dp); break;
    case('m'):
      GUI(c); kp_savepic(ent, dp); break;
    case('1'):
      GUI(c); kp_rl_dec(ent, dp); break;
    case('2'):
      GUI(c); kp_rl_inc(ent, dp); break;
    case('3'):
      GUI(c); kp_r_dec(ent, dp); break;
    case('4'):
      GUI(c); kp_r_inc(ent, dp); break;
    case('l'):
      GUI(c); kp_l_toggle(ent, dp); break;
    case('b'):
      GUI(c); kp_b_toggle(ent, dp); break;
    case('t'):
      GUI(c); kp_t_toggle(ent, dp); break;
    case('n'):
      GUI(c); kp_n_toggle(ent, dp); break;
    case('+'):
      GUI(c); kp_zoom_in(ent, dp); break;
    case('-'):
      GUI(c); kp_zoom_out(ent, dp); break;
    case('q'):
      GUI(c); dp->ui.closed = MUST_CLEANUP; break;

    case('p'):
      kp_print2fig(ent, dp); break;
    case('z'):
      kp_print_xyz(ent, dp); break;
    case('x'):
      kp_print(ent, dp); break;
    case('u'):
      kp_printrot(ent, dp); break;
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
      PRINT_WARN("Unknown command `%c`\n", c);
  }
  return;
}

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
    run_command(c, dp, ent);
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
