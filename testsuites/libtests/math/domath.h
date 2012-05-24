/*
 * Copyright (c) 2010, 2011 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <math.h>
#include <stdio.h>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

#define CONCAT(x, y) x ## y
#define XCONCAT(x, y) CONCAT(x, y)

#define STRINGIFY(x, y) # x # y
#define XSTRINGIFY(x, y) STRINGIFY(x, y)

#ifdef SUFFIX
  #define FUNC(name) XCONCAT(name, SUFFIX)
  #define STR(name) XSTRINGIFY(name, SUFFIX)
#else
  #define FUNC(name) XCONCAT(name, )
  #define STR(name) XSTRINGIFY(name, ) " "
#endif

extern void FUNC(domath) (void);

void
FUNC(domath) (void)
{
#ifndef PROVIDE_EMPTY_FUNC
  FTYPE f1;
  FTYPE f2;

  int i1;

  f1 = FUNC(acos) (0.0);
  fprintf( stdout, STR(acos) "          : " PRI "\n", f1);

  f1 = FUNC(acosh) (0.0);
  fprintf( stdout, STR(acosh) "         : " PRI "\n", f1);

  f1 = FUNC(asin) (1.0);
  fprintf( stdout, STR(asin) "          : " PRI "\n", f1);

  f1 = FUNC(asinh) (1.0);
  fprintf( stdout, STR(asinh) "         : " PRI "\n", f1);

  f1 = FUNC(atan) (M_PI_4);
  fprintf( stdout, STR(atan) "          : " PRI "\n", f1);

  f1 = FUNC(atan2) (2.3, 2.3);
  fprintf( stdout, STR(atan2) "         : " PRI "\n", f1);

  f1 = FUNC(atanh) (1.0);
  fprintf( stdout, STR(atanh) "         : " PRI "\n", f1);

  f1 = FUNC(cbrt) (27.0);
  fprintf( stdout, STR(cbrt) "          : " PRI "\n", f1);

  f1 = FUNC(ceil) (3.5);
  fprintf( stdout, STR(ceil) "          : " PRI "\n", f1);

  f1 = FUNC(copysign) (3.5, -2.5);
  fprintf( stdout, STR(copysign) "      : " PRI "\n", f1);

  f1 = FUNC(cos) (M_PI_2);
  fprintf( stdout, STR(cos) "           : " PRI "\n", f1);

  f1 = FUNC(cosh) (M_PI_2);
  fprintf( stdout, STR(cosh) "          : " PRI "\n", f1);

  f1 = FUNC(erf) (42.0);
  fprintf( stdout, STR(erf) "           : " PRI "\n", f1);

  f1 = FUNC(erfc) (42.0);
  fprintf( stdout, STR(erfc) "          : " PRI "\n", f1);

  f1 = FUNC(exp) (0.42);
  fprintf( stdout, STR(exp) "           : " PRI "\n", f1);

  f1 = FUNC(exp2) (0.42);
  fprintf( stdout, STR(exp2) "          : " PRI "\n", f1);

  f1 = FUNC(expm1) (0.00042);
  fprintf( stdout, STR(expm1) "         : " PRI "\n", f1);

  f1 = FUNC(fabs) (-1.123);
  fprintf( stdout, STR(fabs) "          : " PRI "\n", f1);

  f1 = FUNC(fdim) (1.123, 2.123);
  fprintf( stdout, STR(fdim) "          : " PRI "\n", f1);

  f1 = FUNC(floor) (0.5);
  fprintf( stdout, STR(floor) "         : " PRI "\n", f1);
  f1 = FUNC(floor) (-0.5);
  fprintf( stdout, STR(floor) "         : " PRI "\n", f1);

  f1 = FUNC(fma) (2.1, 2.2, 3.01);
  fprintf( stdout, STR(fma) "           : " PRI "\n", f1);

  f1 = FUNC(fmax) (-0.42, 0.42);
  fprintf( stdout, STR(fmax) "          : " PRI "\n", f1);

  f1 = FUNC(fmin) (-0.42, 0.42);
  fprintf( stdout, STR(fmin) "          : " PRI "\n", f1);

  f1 = FUNC(fmod) (42.0, 3.0);
  fprintf( stdout, STR(fmod) "          : " PRI "\n", f1);

  /* no type-specific variant */
  i1 = fpclassify(1.0);
  fprintf( stdout, "fpclassify     : %d\n", i1);

  f1 = FUNC(frexp) (42.0, &i1);
  fprintf( stdout, STR(frexp) "         : " PRI "\n", f1);

  f1 = FUNC(hypot) (42.0, 42.0);
  fprintf( stdout, STR(hypot) "         : " PRI "\n", f1);

  i1 = FUNC(ilogb) (42.0);
  fprintf( stdout, STR(ilogb) "         : %d\n", i1);

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

  f1 = FUNC(j0) (1.2);
  fprintf( stdout, STR(j0) "            : " PRI "\n", f1);

  f1 = FUNC(j1) (1.2);
  fprintf( stdout, STR(j1) "            : " PRI "\n", f1);

  f1 = FUNC(jn) (2,1.2);
  fprintf( stdout, STR(jn) "            : " PRI "\n", f1);

  f1 = FUNC(ldexp) (1.2,3);
  fprintf( stdout, STR(ldexp) "         : " PRI "\n", f1);

  f1 = FUNC(lgamma) (42.0);
  fprintf( stdout, STR(lgamma) "        : " PRI "\n", f1);

  f1 = FUNC(llrint) (-0.5);
  fprintf( stdout, STR(llrint) "        : " PRI "\n", f1);
  f1 = FUNC(llrint) (0.5);
  fprintf( stdout, STR(llrint) "        : " PRI "\n", f1);

  f1 = FUNC(llround) (-0.5);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);
  f1 = FUNC(llround) (0.5);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);

  f1 = FUNC(log) (42.0);
  fprintf( stdout, STR(log) "           : " PRI "\n", f1);

  f1 = FUNC(log10) (42.0);
  fprintf( stdout, STR(log10) "         : " PRI "\n", f1);

  f1 = FUNC(log1p) (42.0);
  fprintf( stdout, STR(log1p) "         : " PRI "\n", f1);

  f1 = FUNC(log2) (42.0);
  fprintf( stdout, STR(log2) "          : " PRI "\n", f1);

  f1 = FUNC(logb) (42.0);
  fprintf( stdout, STR(logb) "          : " PRI "\n", f1);

  f1 = FUNC(lrint) (-0.5);
  fprintf( stdout, STR(lrint) "         : " PRI "\n", f1);
  f1 = FUNC(lrint) (0.5);
  fprintf( stdout, STR(lrint) "         : " PRI "\n", f1);

  f1 = FUNC(lround) (-0.5);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);
  f1 = FUNC(lround) (0.5);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);

  f1 = FUNC(modf) (42.0,&f2);
  fprintf( stdout, STR(lmodf) "         : " PRI "\n", f1);

  f1 = FUNC(nan) ("");
  fprintf( stdout, STR(nan) "           : " PRI "\n", f1);

  f1 = FUNC(nearbyint) (1.5);
  fprintf( stdout, STR(nearbyint) "     : " PRI "\n", f1);

  f1 = FUNC(nextafter) (1.5,2.0);
  fprintf( stdout, STR(nextafter) "     : " PRI "\n", f1);

  f1 = FUNC(pow) (3.01, 2.0);
  fprintf( stdout, STR(pow) "           : " PRI "\n", f1);

  f1 = FUNC(remainder) (3.01,2.0);
  fprintf( stdout, STR(remainder) "     : " PRI "\n", f1);

  f1 = FUNC(remquo) (29.0,3.0,&i1);
  fprintf( stdout, STR(remquo) "        : " PRI "\n", f1);

  f1 = FUNC(rint) (0.5);
  fprintf( stdout, STR(rint) "          : " PRI "\n", f1);
  f1 = FUNC(rint) (-0.5);
  fprintf( stdout, STR(rint) "          : " PRI "\n", f1);

  f1 = FUNC(round) (0.5);
  fprintf( stdout, STR(round) "         : " PRI "\n", f1);
  f1 = FUNC(round) (-0.5);
  fprintf( stdout, STR(round) "         : " PRI "\n", f1);

  f1 = FUNC(scalbln) (1.2,3);
  fprintf( stdout, STR(scalbln) "       : " PRI "\n", f1);

  f1 = FUNC(scalbn) (1.2,3);
  fprintf( stdout, STR(scalbn) "        : " PRI "\n", f1);

  /* no type-specific variant */
  i1 = signbit(1.0);
  fprintf( stdout, "signbit        : %i\n", i1);

  f1 = FUNC(sin) (M_PI_4);
  fprintf( stdout, STR(sin) "           : " PRI "\n", f1);

  f1 = FUNC(sinh) (M_PI_4);
  fprintf( stdout, STR(sinh) "          : " PRI "\n", f1);

  f1 = FUNC(sqrt) (9.0);
  fprintf( stdout, STR(sqrt) "          : " PRI "\n", f1);

  f1 = FUNC(tan) (M_PI_4);
  fprintf( stdout, STR(tan) "           : " PRI "\n", f1);

  f1 = FUNC(tanh) (M_PI_4);
  fprintf( stdout, STR(tanh) "          : " PRI "\n", f1);

  f1 = FUNC(tgamma) (2.1);
  fprintf( stdout, STR(tgamma) "        : " PRI "\n", f1);

  f1 = FUNC(trunc) (3.5);
  fprintf( stdout, STR(trunc) "         : " PRI "\n", f1);

  f1 = FUNC(y0) (1.2);
  fprintf( stdout, STR(y0) "            : " PRI "\n", f1);

  f1 = FUNC(y1) (1.2);
  fprintf( stdout, STR(y1) "            : " PRI "\n", f1);

  f1 = FUNC(yn) (3,1.2);
  fprintf( stdout, STR(yn) "            : " PRI "\n", f1);
#endif
}
