#include "v.h"
#include "evr.h"
#include "vec3.h"
#include "3d.h"

void kp_readmore(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){
    if(!dp->read.f){
      PRINT_ERR("cannot read from the file '%s'\n", dp->read.fname);
      return;
    }
    fseek(dp->read.f, 0, SEEK_CUR);
    acs_readmore(dp->read, dp->rend.bonds, dp->geom, ent);
    dp->N = ent->n;
    redraw_ac3 (ent, dp);
  }
  return;
}

void kp_readagain(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){

    if(!dp->read.f || !(fclose(dp->read.f), dp->read.f = fopen(dp->read.fname, "r"))){
      PRINT_WARN("cannot reload the file '%s'\n", dp->read.fname);
      return;
    }

    for(int i=0; i<ent->n; i++){
      free(ent->m[i]);
    }
    ent->n = dp->N = dp->n = 0;
    acs_readmore(dp->read, dp->rend.bonds, dp->geom, ent);
    dp->N = ent->n;
    redraw_ac3 (ent, dp);
  }
  return;
}

void kp_print(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    ac3_print(ent->m[dp->n], &dp->rend);
  }
  return;
}

void kp_print_xyz(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    ac3_print_xyz(ent->m[dp->n], &dp->rend, &dp->cell);
  }
  return;
}

void kp_print2fig(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    ac3_print2fig(ent->m[dp->n], &dp->rend, &dp->cell);
  }
  return;
}

void kp_printrot(object * ent __attribute__ ((unused)), drawpars * dp){
  double * U = dp->rend.ac3rmx;
  for(int i=0; i<3; i++){
    PRINTOUT(stdout, "rotation> % 20.15lf % 20.15lf % 20.15lf\n", U[i*3], U[i*3+1], U[i*3+2]);
  }
  PRINTOUT(stdout, "rot:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n\n",
      U[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7], U[8]);
  return;
}

static void rl_changed(object * ent, drawpars * dp){
  for(int i=0; i<ent->n; i++){
    ent->m[i]->bonds.flag = 0;
  }
  exp_redraw(ent, dp);
  return;
}

void kp_rl_dec(object * ent, drawpars * dp){
  if(dp->rend.bonds>0){
    dp->bond.rl /= STEP_R;
    rl_changed(ent, dp);
  }
  return;
}

void kp_rl_inc(object * ent, drawpars * dp){
  if(dp->rend.bonds>0){
    dp->bond.rl *= STEP_R;
    rl_changed(ent, dp);
  }
  return;
}

void kp_r_dec(object * ent, drawpars * dp){
  dp->rend.r /= STEP_R;
  exp_redraw(ent, dp);
  return;
}

void kp_r_inc(object * ent, drawpars * dp){
  dp->rend.r *= STEP_R;
  exp_redraw(ent, dp);
  return;
}

void kp_zoom_out(object * ent, drawpars * dp){
  dp->rend.scale /= STEP_ZOOM;
  exp_redraw(ent, dp);
  return;
}

void kp_zoom_in(object * ent, drawpars * dp){
  dp->rend.scale *= STEP_ZOOM;
  exp_redraw(ent, dp);
  return;
}

void kp_frame_inc(object * ent, drawpars * dp){
  if (dp->n < dp->N-1){
    dp->n++;
    exp_redraw(ent, dp);
  }
  if (dp->n == dp->N-1 && dp->task == AT3COORDS){
    dp->anim.dir = 0;
  }
  return;
}

void kp_frame_dec(object * ent, drawpars * dp){
  if (dp->n > 0){
    dp->n--;
    exp_redraw(ent, dp);
  }
  if (dp->n == 0 && dp->task == AT3COORDS){
    dp->anim.dir = 0;
  }
  return;
}

void rot_ent_pointer(object * ent __attribute__ ((unused)), drawpars * dp, int dx, int dy, double speed){
  double mx[9];
  rot_around_perp(mx, (double)dx, (double)dy, speed);
  mx3_lmultmx(mx, dp->rend.ac3rmx);
  for(int i=0; i<ent->n; i++){
    ent->m[i]->rotated = 0;
  }
  return;
}

static void rot_ent(object * ent __attribute__ ((unused)), drawpars * dp, int axis, double angle){
  if(dp->ui.modkey){
    angle *= STEP_MOD;
  }
  rotmx0_update(dp->rend.ac3rmx, angle, axis);
  for(int i=0; i<ent->n; i++){
    ent->m[i]->rotated = 0;
  }
  return;
}

void kp_rotx_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 0, +STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

void kp_rotx_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 0, -STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

void kp_roty_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 1, +STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

void kp_roty_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 1, -STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

void kp_rotz_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 2, +STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

void kp_rotz_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 2, -STEP_ROT);
  exp_redraw(ent, dp);
  return;
}

static void mol2cell(double r[3], cellpars * cell){
  double rcell[3];
  r3mx(rcell, r, cell->rot_to_cell_basis);
  for(int i=0; i<3; i++){
    if(rcell[i]<-0.5){
      rcell[i] += 1.0;
    }
    else if(rcell[i]>0.5){
      rcell[i] -= 1.0;
    }
  }
  r3mx(r, rcell, cell->rot_to_lab_basis);
  return;
}

static void move_pbc(object * acs, drawpars * dp, int dir, double d){

  double dr[3], v[3] = {};
  v[dir] = d;  // translation in the view basis
  r3mxt(dr, v, dp->rend.ac3rmx);  // translation in the mol basis.
                                  // not true if the initial "rotation" from CLI is not unitary, but ignore this
  for(int i=0; i<acs->n; i++){
    atcoord * m = acs->m[i];
    for(int j=0; j<m->n; j++){
      double * r = m->r0+j*3;
      r3add(r, dr);
      mol2cell(r, &dp->cell);
      r3cp(m->r+j*3, r);
      m->rotated = 0;
    }
    if(dp->rend.bonds>0){
      m->bonds.flag = 0;
      m->bonds.rl *= RL_MOVE_PBC_SCALE;
    }
  }
  return;
}

static void move_ent(object * ent, drawpars * dp, int dir, double step){
  if(dp->ui.modkey){
    step *= STEP_MOD;
  }
  if(dp->cell.vert == 1){
    move_pbc(ent, dp, dir, step);
  }
  else {
    dp->rend.xy0[dir] += step;
  }
  return;
}

void kp_move_l(object * ent, drawpars * dp){
  move_ent(ent, dp, 0, -STEP_MOVE);
  exp_redraw(ent, dp);
  return;
}

void kp_move_r(object * ent, drawpars * dp){
  move_ent(ent, dp, 0, +STEP_MOVE);
  exp_redraw(ent, dp);
  return;
}

void kp_move_u(object * ent, drawpars * dp){
  move_ent(ent, dp, 1, +STEP_MOVE);
  exp_redraw(ent, dp);
  return;
}

void kp_move_d(object * ent, drawpars * dp){
  move_ent(ent, dp, 1, -STEP_MOVE);
  exp_redraw(ent, dp);
  return;
}

void kp_exit(object * ent, drawpars * dp){
  run_commands(NULL, dp->ui.on_exit, dp, ent);
  obj_free(ent);
  close_x();
  CLOSE0(dp->read.f);
  dp->ui.closed = 1;
}

void kp_fw_toggle(object * ent __attribute__ ((unused)), drawpars * dp){
  dp->anim.dir = (dp->anim.dir == 1) ? 0 : 1;
  return;
}

void kp_bw_toggle(object * ent __attribute__ ((unused)), drawpars * dp){
  if (dp->task == AT3COORDS){
    dp->anim.dir = (dp->anim.dir == -1) ? 0 : -1;
  }
  return;
}

void kp_l_toggle(object * ent, drawpars * dp){
  if(dp->rend.bonds>0){
    dp->rend.bonds = 1+!(dp->rend.bonds-1);
    exp_redraw(ent, dp);
  }
  return;
}

void kp_b_toggle(object * ent, drawpars * dp){
  if(dp->rend.bonds>-1){
    dp->rend.bonds = !dp->rend.bonds;
  }
  exp_redraw(ent, dp);
  return;
}

void kp_n_toggle(object * ent, drawpars * dp){
  dp->rend.num = (dp->rend.num ==  1) ? 0 :  1;
  exp_redraw(ent, dp);
  return;
}

void kp_t_toggle(object * ent, drawpars * dp){
  dp->rend.num = (dp->rend.num == -1) ? 0 : -1;
  exp_redraw(ent, dp);
  return;
}

void kp_goto_last(object * ent, drawpars * dp){
  if (dp->n < dp->N-1){
    dp->n=dp->N-1;
    exp_redraw(ent, dp);
  }
  return;
}

void kp_goto_1st(object * ent, drawpars * dp){
  if (dp->n > 0){
    dp->n = 0;
    exp_redraw(ent, dp);
  }
  return;
}

void exp_redraw(object * ent, drawpars * dp){
  switch (dp->task){
    case AT3COORDS:
      redraw_ac3(ent, dp);
      break;
    case VIBRO:
      redraw_vibro(ent, dp);
      break;
    default:
      break;
  }
  return;
}

void time_gone(object * ent, drawpars * dp){
  if(dp->task == VIBRO){
    if(dp->anim.t >= TMAX){
      dp->anim.t = dp->anim.t-TMAX;
    }
    redraw_vibro(ent, dp);
  }
  return;
}

static void savevib(drawpars * dp, int c){
  char s[STRLEN];
  int  l = (int)(log10( dp->N + 0.5 )) + 1;
  snprintf(s, sizeof(s), "%s_%0*d_%02d.xpm", dp->read.fname, l, dp->n+1, c);
  if(savepic(s)){
    fprintf(stderr, "%s\n", s);
  }
  else{
    PRINT_ERR("cannot save '%s'\n", s);
  }
  return;
}

void kp_savepic(object * ent __attribute__ ((unused)), drawpars * dp){
  char s[STRLEN];
  int  l = (int)(log10(dp->N+0.5))+1;
  atcoord * ac = ent->m[dp->n];
  snprintf(s, sizeof(s), "%s_%0*d.xpm", ac->fname, l, dp->n+1);
  if(savepic(s)){
    fprintf(stderr, "%s\n", s);
  }
  else{
    PRINT_ERR("cannot save '%s'\n", s);
  }
  return;
}

void kp_film(object * ent, drawpars * dp){
  if(dp->task != VIBRO){
    kp_savepic    (ent, dp);
    while(dp->n<dp->N-1){
      kp_frame_inc(ent, dp);
      kp_savepic  (ent, dp);
    }
  }
  else{
    int c = 0;
    dp->anim.t = 0;
    do{
      savevib(dp, c);
      dp->anim.t++;
      time_gone(ent, dp);
    } while(++c<TMAX);
  }
  return;
}

void kp_pg(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){
    atcoord * ac = ent->m[dp->n];
    if(!ac->sym[0]){
      pg(ac, dp->anal.symtol);
      redraw_ac3(ent, dp);
    }
  }
  return;
}

void kp_jump(object * ent, drawpars * dp){
  if(!dp->ui.input){
    dp->ui.input = 1;
    exp_redraw(ent, dp);
  }
  return;
}
