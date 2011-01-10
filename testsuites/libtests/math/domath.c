/*
 * Copyright (c) 2010 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/*
 * Try to compile and link against POSIX math routines.
 */

#include <math.h>
#include <stdio.h>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

void
domath  (void)
{
#ifndef NO_DOUBLE
  double f1;
  double f2;

  int i1;

  f1 = acos (0.0);
  fprintf( stdout, "acos           : %f\n", f1);

  f1 = acosh (0.0);
  fprintf( stdout, "acosh          : %f\n", f1);

  f1 = asin (1.0);
  fprintf( stdout, "asin           : %f\n", f1);

  f1 = asinh (1.0);
  fprintf( stdout, "asinh          : %f\n", f1);

  f1 = atan (M_PI_4);
  fprintf( stdout, "atan           : %f\n", f1);

  f1 = atan2 (2.3, 2.3);
  fprintf( stdout, "atan2          : %f\n", f1);

  f1 = atanh (1.0);
  fprintf( stdout, "atanh          : %f\n", f1);

  f1 = cbrt (27.0);
  fprintf( stdout, "cbrt           : %f\n", f1);

  f1 = ceil (3.5);
  fprintf( stdout, "ceil           : %f\n", f1);

  f1 = copysign (3.5, -2.5);
  fprintf( stdout, "copysign       : %f\n", f1);

  f1 = cos (M_PI_2);
  fprintf( stdout, "cos            : %f\n", f1);

  f1 = cosh (M_PI_2);
  fprintf( stdout, "cosh           : %f\n", f1);

  f1 = erf (42.0);
  fprintf( stdout, "erf            : %f\n", f1);

  f1 = erfc (42.0);
  fprintf( stdout, "erfc           : %f\n", f1);

  f1 = exp (0.42);
  fprintf( stdout, "exp            : %f\n", f1);

  f1 = exp2 (0.42);
  fprintf( stdout, "exp2           : %f\n", f1);

  f1 = expm1 (0.00042);
  fprintf( stdout, "expm1          : %f\n", f1);

  f1 = fabs (-1.123);
  fprintf( stdout, "fabs           : %f\n", f1);

  f1 = fdim (1.123, 2.123);
  fprintf( stdout, "fdim           : %f\n", f1);

  f1 = floor (0.5);
  fprintf( stdout, "floor          : %f\n", f1);
  f1 = floor (-0.5);
  fprintf( stdout, "floor          : %f\n", f1);

  f1 = fma (2.1, 2.2, 3.01);
  fprintf( stdout, "fma            : %f\n", f1);

  f1 = fmax (-0.42, 0.42);
  fprintf( stdout, "fmax           : %f\n", f1);

  f1 = fmin (-0.42, 0.42);
  fprintf( stdout, "fmin           : %f\n", f1);

  f1 = fmod (42.0, 3.0);
  fprintf( stdout, "fmod           : %f\n", f1);

  /* no type-specific variant */
  i1 = fpclassify(1.0);
  fprintf( stdout, "fpclassify     : %d\n", i1);

  f1 = frexp (42.0, &i1);
  fprintf( stdout, "frexp          : %f\n", f1);

  f1 = hypot (42.0, 42.0);
  fprintf( stdout, "hypot          : %f\n", f1);

  i1 = ilogb (42.0);
  fprintf( stdout, "ilogb          : %d\n", i1);

  /* no type-specific variant */
  i1 = isfinite(3.0);
  fprintf( stdout, "isfinite       : %d\n", i1);

  /* no type-specific variant */
  i1 = isgreater(3.0, 3.1);
  fprintf( stdout, "isgreater      : %d\n", i1);

  /* no type-specific variant */
  i1 = isgreaterequal(3.0, 3.1);
  fprintf( stdout, "isgreaterequal : %d\n", i1);

  /* no type-specific variant */
  i1 = isinf(3.0);
  fprintf( stdout, "isinf          : %d\n", i1);

  /* no type-specific variant */
  i1 = isless(3.0, 3.1);
  fprintf( stdout, "isless         : %d\n", i1);

  /* no type-specific variant */
  i1 = islessequal(3.0, 3.1);
  fprintf( stdout, "islessequal    : %d\n", i1);

  /* no type-specific variant */
  i1 = islessgreater(3.0, 3.1);
  fprintf( stdout, "islessgreater  : %d\n", i1);

  /* no type-specific variant */
  i1 = isnan(0.0);
  fprintf( stdout, "isnan          : %d\n", i1);

  /* no type-specific variant */
  i1 = isnormal(3.0);
  fprintf( stdout, "isnormal       : %d\n", i1);

  /* no type-specific variant */
  f1 = isunordered(1.0, 2.0);
  fprintf( stdout, "isunordered    : %d\n", i1);

  f1 = j0 (1.2);
  fprintf( stdout, "j0             : %f\n", f1);

  f1 = j1 (1.2);
  fprintf( stdout, "j1             : %f\n", f1);

  f1 = jn (2,1.2);
  fprintf( stdout, "jn             : %f\n", f1);

  f1 = ldexp (1.2,3);
  fprintf( stdout, "ldexp          : %f\n", f1);

  f1 = lgamma (42.0);
  fprintf( stdout, "lgamma         : %f\n", f1);

  f1 = llrint (-0.5);
  fprintf( stdout, "llrint         : %f\n", f1);
  f1 = llrint (0.5);
  fprintf( stdout, "llrint         : %f\n", f1);

  f1 = llround (-0.5);
  fprintf( stdout, "lround         : %f\n", f1);
  f1 = llround (0.5);
  fprintf( stdout, "lround         : %f\n", f1);

  f1 = log (42.0);
  fprintf( stdout, "log            : %f\n", f1);

  f1 = log10 (42.0);
  fprintf( stdout, "log10          : %f\n", f1);

  f1 = log1p (42.0);
  fprintf( stdout, "log1p          : %f\n", f1);

  f1 = log2 (42.0);
  fprintf( stdout, "log2           : %f\n", f1);

  f1 = logb (42.0);
  fprintf( stdout, "logb           : %f\n", f1);

  f1 = lrint (-0.5);
  fprintf( stdout, "lrint          : %f\n", f1);
  f1 = lrint (0.5);
  fprintf( stdout, "lrint          : %f\n", f1);

  f1 = lround (-0.5);
  fprintf( stdout, "lround         : %f\n", f1);
  f1 = lround (0.5);
  fprintf( stdout, "lround         : %f\n", f1);

  f1 = modf (42.0,&f2);
  fprintf( stdout, "lmodf          : %f\n", f1);

  f1 = nan ("");
  fprintf( stdout, "nan            : %f\n", f1);

  f1 = nearbyint (1.5);
  fprintf( stdout, "nearbyint      : %f\n", f1);

  f1 = nextafter (1.5,2.0);
  fprintf( stdout, "nextafter      : %f\n", f1);

  f1 = pow (3.01, 2.0);
  fprintf( stdout, "pow            : %f\n", f1);

  f1 = remainder (3.01,2.0);
  fprintf( stdout, "remainder      : %f\n", f1);

  f1 = remquo (29.0,3.0,&i1);
  fprintf( stdout, "remquo         : %f\n", f1);

  f1 = rint (0.5);
  fprintf( stdout, "rint           : %f\n", f1);
  f1 = rint (-0.5);
  fprintf( stdout, "rint           : %f\n", f1);

  f1 = round (0.5);
  fprintf( stdout, "round          : %f\n", f1);
  f1 = round (-0.5);
  fprintf( stdout, "round          : %f\n", f1);

  f1 = scalbln (1.2,3);
  fprintf( stdout, "scalbln        : %f\n", f1);

  f1 = scalbn (1.2,3);
  fprintf( stdout, "scalbn         : %f\n", f1);

  /* no type-specific variant */
  i1 = signbit(1.0);
  fprintf( stdout, "signbit        : %i\n", i1);

  f1 = sin (M_PI_4);
  fprintf( stdout, "sin            : %f\n", f1);

  f1 = sinh (M_PI_4);
  fprintf( stdout, "sinh           : %f\n", f1);

  f1 = sqrt (9.0);
  fprintf( stdout, "sqrt           : %f\n", f1);

  f1 = tan (M_PI_4);
  fprintf( stdout, "tan            : %f\n", f1);

  f1 = tanh (M_PI_4);
  fprintf( stdout, "tanh           : %f\n", f1);

  f1 = tgamma (2.1);
  fprintf( stdout, "tgamma         : %f\n", f1);

  f1 = trunc (3.5);
  fprintf( stdout, "trunc          : %f\n", f1);

  f1 = y0 (1.2);
  fprintf( stdout, "y0             : %f\n", f1);

  f1 = y1 (1.2);
  fprintf( stdout, "y1             : %f\n", f1);

  f1 = yn (3,1.2);
  fprintf( stdout, "yn             : %f\n", f1);
#endif
}
