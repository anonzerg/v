#include "v.h"
#include "vec3.h"

#define VIEWPORT_FILL 0.5 // molecule fills this fraction of the viewport

double
ac3_scale (atcoord *ac)
{
  double center[3] = {};
  for (int k = 0; k < ac->n; k++)
    {
      r3add (center, ac->r + 3 * k);
    }
  r3scal (center, 1.0 / ac->n);
  double d2max = 0.0;
  for (int k = 0; k < ac->n; k++)
    {
      double rad = get_radius (ac->q[k]);
      double d2 = r3d2 (center, ac->r + 3 * k);
      d2max = MAX (d2max, d2 + rad * rad);
    }
  if (ac->cell.boundary == SHELL)
    {
      for (int i = 0; i < 2; i++)
        {
          d2max = MAX (d2max, 2 * ac->cell.vertices[i] * ac->cell.vertices[i]);
        }
    }
  else if (ac->cell.boundary == CELL)
    {
      for (int k = 0; k < 8; k++)
        {
          double d2 = r3d2 (center, ac->cell.vertices + 3 * k);
          d2max = MAX (d2max, d2);
        }
    }
  return VIEWPORT_FILL / sqrt (d2max);
}

double
acs_scale (object *acs)
{
  double d2max = ac3_scale (acs->m[0]);
  for (int i = 1; i < acs->n; i++)
    {
      d2max = MIN (ac3_scale (acs->m[i]), d2max);
    }
  return d2max;
}
