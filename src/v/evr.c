#include "v.h"
#include "x.h"
#include "evr.h"
#include "vec3.h"

static const double step_rot  = M_PI/90.0;
static const double step_move = 0.2;
static const double step_zoom = 1.1;
static const double step_r    = 1.1;
static const double step_mod  = 0.03125;
static const double rl_move_pbc_scale = 0.9;

static void redraw_ac3(object * ent, drawpars * dp){
  atcoord * ac = ent->m[dp->n];

  if(dp->b>0 && !ac->bond_flag){
    bonds_fill(dp->rl, dp->bmax, ac);
    ac->bond_flag = 1;
  }

  ac3_draw(ac, dp->r, dp->scale, dp->xy0, dp->b, dp->num);
  ac3_text(ac, dp);

  if(dp->vert == 1){
    double v[24];
    for(int i=0; i<8; i++){
      r3mx (v+3*i, dp->vertices+3*i, dp->ac3rmx);
    }
    drawvertices(v, dp->scale, dp->xy0);
  }
  else if(dp->vert == 2){
    drawshell(dp->vertices[0], dp->vertices[1], dp->scale, dp->xy0);
  }

  return;
}

static void redraw_vibro(object * ent, drawpars * dp){

  atcoord * m  = ent->m[0];
  double  * r0 = ent->vib->r0;
  double  * dr = ent->vib->disp + dp->n * m->n*3;

  if(dp->b>0 && !m->bond_flag){
    bonds_fill(dp->rl, dp->bmax, m);
    m->bond_flag = 1;
  }

  vecsums(m->n*3, m->r, r0, dr, sin( dp->t * 2.0*M_PI/TMAX ) * 0.1*sqrt(m->n) );
  for(int j=0; j<m->n; j++){
    double v[3];
    r3mx(v, m->r+3*j, dp->ac3rmx);
    r3cp(m->r+3*j, v);
  }

  ac3_draw(m, dp->r, dp->scale, dp->xy0, dp->b, dp->num);
  vibro_text(ent->vib, dp);

  return;
}

void kp_readmore(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){
    object * acs = ent;
    if(!dp->f){
      PRINT_ERR("cannot read from the file '%s'\n", dp->fname);
      return;
    }
    fseek(dp->f, 0, SEEK_CUR);
    acs_readmore(dp->f, dp->b, dp->center, dp->inertia, dp->bohr, acs, dp->fname);
    newmol_prep(acs, dp);
    redraw_ac3 (acs, dp);
  }
  return;
}

void kp_readagain(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){

    if(!dp->f || !(fclose(dp->f), dp->f = fopen(dp->fname, "r"))){
      PRINT_WARN("cannot reload the file '%s'\n", dp->fname);
      return;
    }

    object * acs = ent;
    for(int i=0; i<acs->n; i++){
      free(acs->m[i]);
    }
    acs->n = dp->N = dp->n = 0;

    acs_readmore(dp->f, dp->b, dp->center, dp->inertia, dp->bohr, acs, dp->fname);
    newmol_prep(acs, dp);
    redraw_ac3 (acs, dp);
  }
  return;
}

void kp_print(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    atcoord * ac = ent->m[dp->n];
    ac3_print(ac, dp->xy0, dp->b);
  }
  return;
}

void kp_print_xyz(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    atcoord * ac = ent->m[dp->n];
    ac3_print_xyz(ac, dp->xy0);
  }
  return;
}

void kp_printrot(object * ent __attribute__ ((unused)), drawpars * dp){
  double * U = dp->ac3rmx;
  for(int i=0; i<3; i++){
    PRINTOUT(stdout, "rotation> % 20.15lf % 20.15lf % 20.15lf\n", U[i*3], U[i*3+1], U[i*3+2]);
  }
  PRINTOUT(stdout, "rot:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n\n",
      U[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7], U[8]);
  return;
}

void kp_print2fig(object * ent, drawpars * dp){
  if (dp->task == AT3COORDS){
    double v[3*8];
    if(dp->vert == 1){
      for(int i=0; i<8; i++){
        r3mx (v+3*i, dp->vertices+3*i, dp->ac3rmx);
      }
    }
    atcoord * ac = ent->m[dp->n];
    ac3_print2fig(ac, dp->xy0, dp->b, dp->vert==1?v:NULL);
  }
  return;
}

static void rl_changed(object * ent, drawpars * dp){
  for(int i=0; i<ent->n; i++){
    ent->m[i]->bond_flag = 0;
  }
  exp_redraw(ent, dp);
  return;
}

void kp_rl_dec(object * ent, drawpars * dp){
  if(dp->b>0){
    dp->rl /= step_r;
    rl_changed(ent, dp);
  }
  return;
}

void kp_rl_inc(object * ent, drawpars * dp){
  if(dp->b>0){
    dp->rl *= step_r;
    rl_changed(ent, dp);
  }
  return;
}

void kp_r_dec(object * ent, drawpars * dp){
  dp->r /= step_r;
  exp_redraw(ent, dp);
  return;
}

void kp_r_inc(object * ent, drawpars * dp){
  dp->r *= step_r;
  exp_redraw(ent, dp);
  return;
}

void kp_zoom_out(object * ent, drawpars * dp){
  dp->scale /= step_zoom;
  exp_redraw(ent, dp);
  return;
}

void kp_zoom_in(object * ent, drawpars * dp){
  dp->scale *= step_zoom;
  exp_redraw(ent, dp);
  return;
}

void kp_frame_inc(object * ent, drawpars * dp){
  if (dp->n < dp->N-1){
    dp->n++;
    exp_redraw(ent, dp);
  }
  if (dp->n == dp->N-1 && dp->task == AT3COORDS){
    dp->fbw = 0;
  }
  return;
}

void kp_frame_dec(object * ent, drawpars * dp){
  if (dp->n > 0){
    dp->n--;
    exp_redraw(ent, dp);
  }
  if (dp->n == 0 && dp->task == AT3COORDS){
    dp->fbw = 0;
  }
  return;
}

void rot_ent_pointer(object * ent, drawpars * dp, int dx, int dy, double speed){

  double rotation_matrix[9];
  rot_around_perp(rotation_matrix, (double)dx, (double)dy, speed);

  double mx0[9];
  veccp(9, mx0, dp->ac3rmx);
  mx_multmx(3,3,3, dp->ac3rmx, rotation_matrix, mx0);
  if(dp->task == AT3COORDS){
    object * acs = ent;
    for(int i=0; i<dp->N; i++){
      rot3d(acs->m[i]->n, acs->m[i]->r, rotation_matrix);
    }
  }
  return;
}

static void rot_ent(object * ent, drawpars * dp, int axis, double angle){
  if(dp->modkey){
    angle *= step_mod;
  }

  double m[9];
  rotmx0_update(dp->ac3rmx, m, angle, axis);

  if(dp->task == AT3COORDS){
    object * acs = ent;
    for(int i=0; i<dp->N; i++){
      rot3d(acs->m[i]->n, acs->m[i]->r, m);
    }
  }
  return;
}

void kp_rotx_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 0, +step_rot);
  exp_redraw(ent, dp);
  return;
}

void kp_rotx_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 0, -step_rot);
  exp_redraw(ent, dp);
  return;
}

void kp_roty_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 1, +step_rot);
  exp_redraw(ent, dp);
  return;
}

void kp_roty_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 1, -step_rot);
  exp_redraw(ent, dp);
  return;
}

void kp_rotz_l(object * ent, drawpars * dp){
  rot_ent(ent, dp, 2, +step_rot);
  exp_redraw(ent, dp);
  return;
}

void kp_rotz_r(object * ent, drawpars * dp){
  rot_ent(ent, dp, 2, -step_rot);
  exp_redraw(ent, dp);
  return;
}

static void mol2cell(double r0[3], drawpars * dp){
  double mat[9], r[3];
  veccp(9, mat, dp->ac3rmx);
  r3cp(r, r0);
  mx_inv (3, 1, r, mat, 1e-15);
  double rcell[3];
  r3mx(rcell, r, dp->rot_to_cell_basis);
  for(int i=0; i<3; i++){
    if(rcell[i]<-0.5){
      rcell[i] += 1.0;
    }
    else if(rcell[i]>0.5){
      rcell[i] -= 1.0;
    }
  }
  r3mx(r, rcell, dp->rot_to_lab_basis);
  r3mx(r0, r, dp->ac3rmx);
  return;
}

static void move_pbc(object * acs, drawpars * dp, int dir, double d){
  for(int i=0; i<dp->N; i++){
    for(int j=0; j<acs->m[i]->n; j++){
      double * r = acs->m[i]->r+j*3;
      r[dir] += d;
      mol2cell(r, dp);
    }
    if(dp->b>0){
      acs->m[i]->bond_flag = 0;
      acs->m[i]->bond_rl *= rl_move_pbc_scale;
    }
  }
  return;
}

static void move_ent(object * ent, drawpars * dp, int dir, double step){
  if(dp->modkey){
    step *= step_mod;
  }
  if(dp->vert == 1){
    move_pbc(ent, dp, dir, step);
  }
  else {
    dp->xy0[dir] += step;
  }
  return;
}

void kp_move_l(object * ent, drawpars * dp){
  move_ent(ent, dp, 0, -step_move);
  exp_redraw(ent, dp);
  return;
}

void kp_move_r(object * ent, drawpars * dp){
  move_ent(ent, dp, 0, +step_move);
  exp_redraw(ent, dp);
  return;
}

void kp_move_u(object * ent, drawpars * dp){
  move_ent(ent, dp, 1, +step_move);
  exp_redraw(ent, dp);
  return;
}

void kp_move_d(object * ent, drawpars * dp){
  move_ent(ent, dp, 1, -step_move);
  exp_redraw(ent, dp);
  return;
}

void kp_exit(object * ent, drawpars * dp){
  run_commands(NULL, dp->on_exit, dp, ent);
  obj_free(ent);
  if(dp->f){
    fclose(dp->f);
  }
  dp->closed = 1;
  close_x();
}

void kp_fw_toggle(object * ent __attribute__ ((unused)), drawpars * dp){
  dp->fbw = (dp->fbw == 1) ? 0 : 1;
  return;
}

void kp_bw_toggle(object * ent __attribute__ ((unused)), drawpars * dp){
  if (dp->task == AT3COORDS){
    dp->fbw = (dp->fbw == -1) ? 0 : -1;
  }
  return;
}

void kp_l_toggle(object * ent, drawpars * dp){
  if(dp->b>0){
    dp->b = 1+!(dp->b-1);
    exp_redraw(ent, dp);
  }
  return;
}

void kp_b_toggle(object * ent, drawpars * dp){
  if(dp->b>-1){
    dp->b = !dp->b;
  }
  exp_redraw(ent, dp);
  return;
}

void kp_n_toggle(object * ent, drawpars * dp){
  dp->num = (dp->num ==  1) ? 0 :  1;
  exp_redraw(ent, dp);
  return;
}

void kp_t_toggle(object * ent, drawpars * dp){
  dp->num = (dp->num == -1) ? 0 : -1;
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
    if(dp->t >= TMAX){
      dp->t = dp->t-TMAX;
    }
    redraw_vibro(ent, dp);
  }
  return;
}

static void savevib(drawpars * dp, int c){
  char s[STRLEN];
  int  l = (int)(log10( dp->N + 0.5 )) + 1;
  snprintf(s, sizeof(s), "%s_%0*d_%02d.xpm", dp->fname, l, dp->n+1, c);
  if (savepic(s) != XpmSuccess){
    PRINT_ERR("cannot save '%s'\n", s);
  }
  else{
    fprintf(stderr, "%s\n", s);
  }
  return;
}

void kp_savepic(object * ent __attribute__ ((unused)), drawpars * dp){
  char s[STRLEN];
  int  l = (int)(log10(dp->N+0.5))+1;
  atcoord * ac = ent->m[dp->n];
  snprintf(s, sizeof(s), "%s_%0*d.xpm", ac->fname, l, dp->n+1);
  if (savepic(s) != XpmSuccess){
    PRINT_ERR("cannot save '%s'\n", s);
  }
  else{
    fprintf(stderr, "%s\n", s);
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
    dp->t = 0;
    do{
      savevib(dp, c);
      dp->t++;
      time_gone(ent, dp);
    } while(++c<TMAX);
  }
  return;
}

void kp_pg(object * ent, drawpars * dp){
  if(dp->task == AT3COORDS){
    atcoord * ac = ent->m[dp->n];
    if(!ac->sym[0]){
      pg(ac, dp->symtol);
      redraw_ac3(ent, dp);
    }
  }
  return;
}

void kp_jump(object * ent, drawpars * dp){
  if(!dp->input){
    dp->input = 1;
    exp_redraw(ent, dp);
  }
  return;
}
