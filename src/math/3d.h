#include "matrix.h"

#define DEG2RAD (M_PI/180.0)

void rotmx0_update(double mx[9], double phi, int axis);
void rot3d(int n, double * v, const double * r, const double m[9]);
void rot3d_inplace(int n, double * r, const double m[9]);
void rotmx (double * rot, double * u, double phi);
void rot_around_perp(double rot[9], double dx, double dy, double factor);
void mx3_lmultmx(const double A[9], double B[9]);

int zmat2cart(int n, double r[3],
              double a[3], double b[3], double c[3],
              double R,  double phi, double theta);
