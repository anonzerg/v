#include "matrix.h"

#define DEG2RAD (M_PI/180.0)

void rotmx0_update(double mx[9], double mx1[9], double phi, int axis);
void rot3d (int n, double * r, double m[9]);
void rotmx (double * rot, double * u, double phi);
void rot_around_perp(double rot[9], double dx, double dy, double factor);

int zmat2cart(int n, double r[3],
              double a[3], double b[3], double c[3],
              double R,  double phi, double theta);
