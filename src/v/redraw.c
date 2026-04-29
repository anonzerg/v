#include "evr.h"
#include "v.h"

static void
draw_boundary (atcoord *m, rendpars *rend)
{
  if (m->cell.boundary == CELL)
    {
      double v[8 * 3];
      rot3d (8, v, m->cell.vertices, rend->ac3rmx);
      draw_vertices (v, rend);
    }
  else if (m->cell.boundary == SHELL)
    {
      draw_shell (m->cell.vertices, rend);
    }
  return;
}

static void
screen_text (object *ent, drawpars *dp)
{
  char text[STRLEN];
  char text_fname[STRLEN];
  char text_input[STRLEN];
  char text_coord[STRLEN];
  char text_point[32];
  const char *lines[MAX_LINES] = {};
  int lines_red[MAX_LINES] = {};
  const atcoord *m = ent->m[MOL_IDX (dp)];
  int il = 0;

  if (dp->task == AT3COORDS)
    {
      set_caption (m->fname);
      snprintf (text, sizeof (text), "%*d / %d", 1 + (int)(log10 (dp->N)),
                dp->n + 1, dp->N);
    }
  else
    {
      double fq = ent->vib->freq[dp->n];
      char i = fq > 0.0 ? ' ' : 'i';
      snprintf (text, sizeof (text),
                "%*d / %d   freq = %.1lf%c cm-1   int = %.1lf km/mole   mass "
                "= %.1lf amu",
                1 + (int)(log10 (ent->vib->n)), dp->n + 1, ent->vib->n,
                fabs (fq), i, ent->vib->ints[dp->n], ent->vib->mass[dp->n]);
    }
  lines[il++] = text;

  if (m->nf[1] == ent->n)
    {
      lines[il++] = m->fname;
    }
  else
    {
      snprintf (text_fname, sizeof (text_fname), "%s (%*d / %d)", m->fname,
                1 + (int)(log10 (m->nf[1])), m->nf[0] + 1, m->nf[1]);
      lines[il++] = text_fname;
    }

  if (dp->anal.intcoord[0])
    {
      double z = intcoord_calc (1, m->n, dp->anal.intcoord, m->r);
      switch (dp->anal.intcoord[0])
        {
        case 1:
          snprintf (text_coord, sizeof (text_coord), "bond %d-%d: %.3lf",
                    dp->anal.intcoord[1], dp->anal.intcoord[2], z);
          break;
        case 2:
          snprintf (text_coord, sizeof (text_coord), "angle %d-%d-%d: %.1lf",
                    dp->anal.intcoord[1], dp->anal.intcoord[2],
                    dp->anal.intcoord[3], z);
          break;
        case 3:
          snprintf (text_coord, sizeof (text_coord),
                    "dihedral %d-%d-%d-%d: % .1lf", dp->anal.intcoord[1],
                    dp->anal.intcoord[2], dp->anal.intcoord[3],
                    dp->anal.intcoord[4], z);
          break;
        default:
          break;
        }
      lines[il++] = text_coord;
    }

  if (m->sym[0])
    {
      snprintf (text_point, sizeof (text_point), "%spoint group: %s",
                dp->task == AT3COORDS ? "" : "molecule ", m->sym);
      lines[il++] = text_point;
    }

  if (dp->ui.input == INPUT_JUMP)
    {
      snprintf (text_input, sizeof (text_input), "JUMP TO >>> %s",
                dp->ui.input_text);
      lines_red[il] = 1;
      lines[il++] = text_input;
    }

  put_text (lines, lines_red);
  return;
}

void
exp_redraw (object *ent, drawpars *dp)
{
  atcoord *m = ent->m[MOL_IDX (dp)];
  fill_bonds (m, dp);
  if (dp->task == AT3COORDS)
    {
      rotate_mol (m, dp);
      if (m->cell.boundary == CELL)
        {
          dp->rend.xy0[0] = 0.0;
          dp->rend.xy0[1] = 0.0;
        }
    }
  else if (dp->task == VIBRO)
    {
      const double *dr = ent->vib->disp + dp->n * m->n * 3;
      vecsums (m->n * 3, m->r, m->r0, dr,
               VIBR_AMP * sqrt (m->n) * sin (dp->anim.t * 2.0 * M_PI / TMAX));
      rot3d_inplace (m->n, m->r, dp->rend.ac3rmx);
    }

  if (dp->ui.gui != 1)
    {
      return;
    }

  clear_canv ();
  screen_text (ent, dp);
  ac3_draw (m, &dp->rend);
  draw_boundary (m, &dp->rend);
  fill_canv ();
  return;
}
