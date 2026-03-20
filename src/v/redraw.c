#include "v.h"
#include "evr.h"

static void draw_boundary(atcoord * m , rendpars * rend){
  if(m->cell.boundary==CELL){
    double v[8*3];
    rot3d(8, v, m->cell.vertices, rend->ac3rmx);
    draw_vertices(v, rend);
  }
  else if(m->cell.boundary==SHELL){
    draw_shell(m->cell.vertices, rend);
  }
  return;
}

static void ac3_text(atcoord * ac, drawpars * dp){
  char text[32];
  char text_fname[STRLEN];
  char text_input[STRLEN];
  char text_coord[STRLEN];
  char text_point[32];

  const char * lines[MAX_LINES] = {};
  int lines_red[MAX_LINES] = {};

  int il=0;
  snprintf(text, sizeof(text), "%*d / %d", 1+(int)(log10(dp->N)), dp->n+1, dp->N);
  lines[il++] = text;

  if(ac->nf[1]==dp->N){
    lines[il++] = dp->read.fname;
  }
  else{
    snprintf(text_fname, sizeof(text_fname), "%s (%*d / %d)", ac->fname, 1+(int)(log10(ac->nf[1])), ac->nf[0]+1, ac->nf[1]);
    lines[il++] = text_fname;
  }

  if(dp->anal.intcoord[0]){
    double z = intcoord_calc(1, ac->n, dp->anal.intcoord, ac->r);
    switch(dp->anal.intcoord[0]){
      case 1:
        snprintf(text_coord, sizeof(text_coord), "bond %d-%d: %.3lf", dp->anal.intcoord[1], dp->anal.intcoord[2], z);
        break;
      case 2:
        snprintf(text_coord, sizeof(text_coord), "angle %d-%d-%d: %.1lf", dp->anal.intcoord[1], dp->anal.intcoord[2], dp->anal.intcoord[3], z);
        break;
      case 3:
        snprintf(text_coord, sizeof(text_coord), "dihedral %d-%d-%d-%d: % .1lf", dp->anal.intcoord[1], dp->anal.intcoord[2], dp->anal.intcoord[3], dp->anal.intcoord[4], z);
        break;
      default:
        break;
    }
    lines[il++] = text_coord;
  }

  if(ac->sym[0]){
    snprintf(text_point, sizeof(text_point), "point group: %s", ac->sym);
    lines[il++] = text_point;
  }

  if(dp->ui.input==1){
    snprintf(text_input, sizeof(text_input), "JUMP TO >>> %s", dp->ui.input_text);
    lines_red[il] = 1;
    lines[il++] = text_input;
  }

  textincorner(lines, lines_red);
  setcaption(ac->fname);
  return;
}

void vibro_text(vibr_t * ms, drawpars * dp){
  char text[STRLEN], text_inp[STRLEN];
  const char * lines[MAX_LINES] = {[0] = text, [1] = dp->read.fname};
  const int lines_red[MAX_LINES] = {[2] = 1};

  double fq = ms->freq[dp->n];
  char i = fq > 0.0 ? ' ' : 'i';
  snprintf(text, sizeof(text),
           "%*d / %d   freq = %.1lf%c cm-1   int = %.1lf km/mole   mass = %.1lf amu",
           1+(int)(log10(ms->n)), dp->n+1, ms->n, fabs(fq), i, ms->ints[dp->n], ms->mass[dp->n]);
  if(dp->ui.input==1){
    snprintf(text_inp, sizeof(text_inp), "JUMP TO >>> %s", dp->ui.input_text);
    lines[2] = text_inp;
  }
  textincorner(lines, lines_red);
  return;
}

void redraw_ac3(object * ent, drawpars * dp){
  atcoord * m = ent->m[dp->n];
  fill_bonds(m, dp);
  rotate_mol(m, dp);
  if(m->cell.boundary==CELL){
    dp->rend.xy0[0] = 0.0;
    dp->rend.xy0[1] = 0.0;
  }

  clear_canv();
  ac3_draw(m, &dp->rend);
  ac3_text(m, dp);
  draw_boundary(m, &dp->rend);
  fill_canv();
  return;
}

void redraw_vibro(object * ent, drawpars * dp){
  atcoord * m  = ent->m[0];
  fill_bonds(m, dp);
  double  * dr = ent->vib->disp + dp->n * m->n*3;
  vecsums(m->n*3, m->r, m->r0, dr, VIBR_AMP*sqrt(m->n)*sin(dp->anim.t * 2.0*M_PI/TMAX));
  rot3d_inplace(m->n, m->r, dp->rend.ac3rmx);

  clear_canv();
  ac3_draw(m, &dp->rend);
  vibro_text(ent->vib, dp);
  draw_boundary(m, &dp->rend);
  fill_canv();
  return;
}
