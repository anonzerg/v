#include "3d.h"

#define TMAX 20
#define VIBR_AMP   0.1
#define STEP_ROT  (M_PI/90.0)
#define STEP_MOVE  0.2
#define STEP_ZOOM  1.1
#define STEP_R     1.1
#define STEP_MOD   0.03125
#define RL_MOVE_PBC_SCALE 0.9

#define MOL_IDX(DP) (DP->task == AT3COORDS ? DP->n : 0)

static inline void fill_bonds(atcoord * m, const drawpars * dp){
  if(dp->rend.bonds>0){
    bonds_fill(dp->bond, m);
  }
  return;
}

static inline void rotate_mol(atcoord * m, const drawpars * dp){
  if(!m->rotated){
    rot3d(m->n, m->r, m->r0, dp->rend.ac3rmx);
    m->rotated = 1;
  }
  return;
}

void kp_readmore (object * ent, drawpars * dp);
void kp_readagain(object * ent, drawpars * dp);
void kp_print    (object * ent, drawpars * dp);
void kp_print_xyz(object * ent, drawpars * dp);
void kp_print2fig(object * ent, drawpars * dp);
void kp_printrot (object * ent, drawpars * dp);
void kp_rl_inc   (object * ent, drawpars * dp);
void kp_rl_dec   (object * ent, drawpars * dp);
void kp_r_inc    (object * ent, drawpars * dp);
void kp_r_dec    (object * ent, drawpars * dp);
void kp_zoom_out (object * ent, drawpars * dp);
void kp_zoom_in  (object * ent, drawpars * dp);
void kp_lat_dec  (object * ent, drawpars * dp);
void kp_lat_inc  (object * ent, drawpars * dp);
void kp_frame_inc(object * ent, drawpars * dp);
void kp_frame_dec(object * ent, drawpars * dp);
void kp_rotz_l   (object * ent, drawpars * dp);
void kp_rotz_r   (object * ent, drawpars * dp);
void kp_roty_l   (object * ent, drawpars * dp);
void kp_roty_r   (object * ent, drawpars * dp);
void kp_rotx_l   (object * ent, drawpars * dp);
void kp_rotx_r   (object * ent, drawpars * dp);
void kp_move_l   (object * ent, drawpars * dp);
void kp_move_r   (object * ent, drawpars * dp);
void kp_move_u   (object * ent, drawpars * dp);
void kp_move_d   (object * ent, drawpars * dp);
void kp_exit     (object * ent, drawpars * dp);
void kp_l_toggle (object * ent, drawpars * dp);
void kp_b_toggle (object * ent, drawpars * dp);
void kp_t_toggle (object * ent, drawpars * dp);
void kp_n_toggle (object * ent, drawpars * dp);
void kp_fw_toggle(object * ent, drawpars * dp);
void kp_bw_toggle(object * ent, drawpars * dp);
void kp_goto_last(object * ent, drawpars * dp);
void kp_goto_1st (object * ent, drawpars * dp);
void time_gone   (object * ent, drawpars * dp);
void kp_save_pic (object * ent, drawpars * dp);
void kp_film     (object * ent, drawpars * dp);
void kp_pg       (object * ent, drawpars * dp);
void kp_jump     (object * ent, drawpars * dp);
void rot_ent_pointer(object * ent, drawpars * dp, int dx, int dy, double speed);
