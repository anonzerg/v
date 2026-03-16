#include "v.h"
#include "evr.h"
#include "vec3.h"
#include "vecn.h"

static void ac3_text(atcoord * ac, drawpars * dp){
  char text[STRLEN], text2[STRLEN], text_inp[STRLEN];
  const char * lines[MAX_LINES] = {[0] = text, [1] = dp->read.fname};
  const int lines_red[MAX_LINES] = {[2] = 1};

  int tp = snprintf(text, sizeof(text), "%*d / %d   r = %.1lf   rl = %.1lf", 1+(int)(log10(dp->N)), dp->n+1, dp->N, dp->rend.r, dp->bond.rl);
  if( tp<sizeof(text)-1 && dp->anal.intcoord[0] ){
    tp += snprintf(text+tp, sizeof(text)-tp, "  |  %d,%d,%d,%d,%d: %lf", dp->anal.intcoord[0], dp->anal.intcoord[1], dp->anal.intcoord[2], dp->anal.intcoord[3], dp->anal.intcoord[4], intcoord_calc(1, ac->n, dp->anal.intcoord, ac->r));
  }
  if( tp<sizeof(text)-1 && ac->sym[0] ){
    tp += snprintf(text+tp, sizeof(text)-tp, "  |  PG: %s", ac->sym);
  }

  if(ac->nf[1]!=dp->N){
    snprintf(text2, sizeof(text2), "%s (%*d / %d)", ac->fname, 1+(int)(log10(ac->nf[1])), ac->nf[0]+1, ac->nf[1]);
    lines[1] = text2;
  }

  if(dp->ui.input==1){
    snprintf(text_inp, sizeof(text_inp), "JUMP TO >>> %s", dp->ui.input_text);
    lines[2] = text_inp;
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
           "%*d / %d   freq = %.1lf%c cm-1   int = %.1lf km/mole   mass = %.1lf amu  |  r = %.1lf   rl = %.1lf",
           1+(int)(log10(ms->n)), dp->n+1, ms->n, fabs(fq), i, ms->ints[dp->n], ms->mass[dp->n], dp->rend.r, dp->bond.rl);
  if(dp->ui.input==1){
    snprintf(text_inp, sizeof(text_inp), "JUMP TO >>> %s", dp->ui.input_text);
    lines[2] = text_inp;
  }
  textincorner(lines, lines_red);
  return;
}

void redraw_ac3(object * ent, drawpars * dp){
  atcoord * ac = ent->m[dp->n];

  if(dp->rend.bonds>0){
    bonds_fill(dp->bond, ac);
  }

  clear_canv();
  ac3_draw(ac, dp->rend);
  ac3_text(ac, dp);

  if(dp->cell.vert == 1){
    double v[24];
    for(int i=0; i<8; i++){
      r3mx (v+3*i, dp->cell.vertices+3*i, dp->rend.ac3rmx);
    }
    drawvertices(v, dp->rend.scale, dp->rend.xy0);
  }
  else if(dp->cell.vert == 2){
    drawshell(dp->cell.vertices[0], dp->cell.vertices[1], dp->rend.scale, dp->rend.xy0);
  }
  fill_canv();

  return;
}

void redraw_vibro(object * ent, drawpars * dp){

  atcoord * m  = ent->m[0];
  double  * r0 = ent->vib->r0;
  double  * dr = ent->vib->disp + dp->n * m->n*3;

  if(dp->rend.bonds>0){
    bonds_fill(dp->bond, m);
  }

  vecsums(m->n*3, m->r, r0, dr, sin( dp->anim.t * 2.0*M_PI/TMAX ) * VIBR_AMP*sqrt(m->n) );
  for(int j=0; j<m->n; j++){
    double v[3];
    r3mx(v, m->r+3*j, dp->rend.ac3rmx);
    r3cp(m->r+3*j, v);
  }

  clear_canv();
  ac3_draw(m, dp->rend);
  vibro_text(ent->vib, dp);
  fill_canv();
  return;
}
