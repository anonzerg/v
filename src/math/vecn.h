#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void vecset (size_t n, double *u, double s);
void vecsums (size_t n, double *w, const double *u, const double *v, double s);
void vecadds (size_t n, double *u, const double *v, double s);
void veccp (size_t n, double *u, const double *v);
void vecscal (size_t n, double *u, double s);
