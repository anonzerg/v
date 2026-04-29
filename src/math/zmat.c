#include "3d.h"
#include "vec3.h"

#define EPS 1e-15

int
zmat2cart (int n, double r[3], const double a[3], const double b[3],
           const double c[3], double R, double phi, double theta)
{

  if (n == 0)
    {
      r[0] = r[1] = r[2] = 0.0;
    }

  else if (n == 1)
    {
      r[0] = a[0];
      r[1] = a[1] + R;
      r[2] = a[2];
    }

  else if (n == 2)
    {
      r[0] = a[0] + R * sqrt (1 - cos (phi) * cos (phi));
      r[1] = a[1] + ((b[1] < a[1]) ? -1.0 : +1.0) * R * cos (phi);
      r[2] = a[2];
    }

  else
    {

      double ab[3], bc[3];
      double r1[3], r2[3];
      double perp[3];
      double rot[9];
      double t;

      r3diff (ab, b, a);
      t = r3dot (ab, ab);
      if (t < EPS)
        {
          return 1;
        }
      r3scal (ab, 1.0 / sqrt (t));

      r3cp (r1, a);
      r3adds (r1, ab, R);

      r3diff (bc, b, c);
      r3x (perp, ab, bc);
      t = r3dot (perp, perp);
      if (t < EPS)
        {
          return 1;
        }
      r3scal (perp, 1.0 / sqrt (t));

      r3min (r1, a);
      rotmx (rot, perp, -phi);
      r3mx (r2, r1, rot);
      rotmx (rot, ab, theta);
      r3mx (r, r2, rot);
      r3add (r, a);
    }

  return 0;
}
