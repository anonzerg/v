#include "v.h"
#include "evr.h"

static void run_with_gui(char c, drawpars * dp, object * ent){
  if(dp->ui.gui!=GUI_DISABLED){
    int gui = dp->ui.gui;
    dp->ui.gui = GUI_ENABLED;
    switch(c){
      case('f'):
        kp_film(ent, dp); break;
      case('m'):
        kp_save_pic(ent, dp); break;
    }
    dp->ui.gui = gui;
  }
  else{
    PRINT_WARN("Ignoring command `%c` in the headless mode\n", c); \
  }
  return;
}

static void run_command(char c, drawpars * dp, object * ent){
  switch(c){
    case('>'):
      kp_fw_toggle(ent, dp); break;
    case('d'):
      kp_move_r(ent, dp); break;
    case('a'):
      kp_move_l(ent, dp); break;
    case('w'):
      kp_move_u(ent, dp); break;
    case('s'):
      kp_move_d(ent, dp); break;
    case('1'):
      kp_rl_dec(ent, dp); break;
    case('2'):
      kp_rl_inc(ent, dp); break;
    case('3'):
      kp_r_dec(ent, dp); break;
    case('4'):
      kp_r_inc(ent, dp); break;
    case('l'):
      kp_l_toggle(ent, dp); break;
    case('b'):
      kp_b_toggle(ent, dp); break;
    case('t'):
      kp_t_toggle(ent, dp); break;
    case('n'):
      kp_n_toggle(ent, dp); break;
    case('+'):
      kp_zoom_in(ent, dp); break;
    case('-'):
      kp_zoom_out(ent, dp); break;

    case('f'):
    case('m'):
      run_with_gui(c, dp, ent); break;

    case('q'):
      dp->ui.closed = MUST_CLEANUP; break;

    case('p'):
      kp_print2fig(ent, dp); break;
    case('z'):
      kp_print_xyz(ent, dp); break;
    case('x'):
      kp_print(ent, dp); break;
    case('u'):
      kp_printrot(ent, dp); break;
    case('.'):
      kp_pg(ent, dp);
      PRINTOUT(stdout, "%s\n", ent->m[dp->n]->sym);
      break;
    case(' '):
    case('\n'):
      break;
    default:
      PRINT_WARN("Unknown command `%c`\n", c);
  }
  return;
}

void run_commands(FILE * f, char * command, drawpars * dp, object * ent){
  const char * com = command;
  int c;

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
