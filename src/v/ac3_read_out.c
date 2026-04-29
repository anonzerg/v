#include "mol.h"
#include "v.h"
#include "vec3.h"

mol *
ac3_read_out (FILE *f)
{

  long pos0 = ftell (f);

  // find where the molecule begins
  char s[STRLEN];
  while (1)
    {
      if (!fgets (s, sizeof (s), f))
        {
          goto hell;
        }
      if (strstr (s, "Atomic Coordinates:"))
        {
          break;
        }
    }

  // count atoms
  long pos1 = ftell (f);
  int n = 0;
  while (read_cart_atom (f, n, NULL))
    {
      n++;
    }
  if (!n)
    {
      goto hell;
    }
  fseek (f, pos1, SEEK_SET);

  // fill in
  mol *m = alloc_mol (n);
  for (int i = 0; i < n; i++)
    {
      read_cart_atom (f, i, m);
    }
  return m;

hell:
  fseek (f, pos0, SEEK_SET);
  return NULL;
}
