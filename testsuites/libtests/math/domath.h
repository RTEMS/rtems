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

volatile int ia;

volatile FTYPE fa;

volatile FTYPE fb;

volatile FTYPE fc;

void
FUNC(domath) (void)
{
#ifndef PROVIDE_EMPTY_FUNC
  FTYPE f1;
  FTYPE f2;

  int i1;

  fa = 0.0;
  f1 = FUNC(acos) (fa);
  fprintf( stdout, STR(acos) "          : " PRI "\n", f1);

  fa = 0.0;
  f1 = FUNC(acosh) (fa);
  fprintf( stdout, STR(acosh) "         : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(asin) (fa);
  fprintf( stdout, STR(asin) "          : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(asinh) (fa);
  fprintf( stdout, STR(asinh) "         : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(atan) (fa);
  fprintf( stdout, STR(atan) "          : " PRI "\n", f1);

  fa = 2.3;
  fb = 2.3;
  f1 = FUNC(atan2) (fa, fb);
  fprintf( stdout, STR(atan2) "         : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(atanh) (fa);
  fprintf( stdout, STR(atanh) "         : " PRI "\n", f1);

  fa = 27.0;
  f1 = FUNC(cbrt) (fa);
  fprintf( stdout, STR(cbrt) "          : " PRI "\n", f1);

  fa = 3.5;
  f1 = FUNC(ceil) (fa);
  fprintf( stdout, STR(ceil) "          : " PRI "\n", f1);

  fa = 3.5;
  fb = -2.5;
  f1 = FUNC(copysign) (fa, fb);
  fprintf( stdout, STR(copysign) "      : " PRI "\n", f1);

  fa = M_PI_2;
  f1 = FUNC(cos) (fa);
  fprintf( stdout, STR(cos) "           : " PRI "\n", f1);

  fa = M_PI_2;
  f1 = FUNC(cosh) (fa);
  fprintf( stdout, STR(cosh) "          : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(erf) (fa);
  fprintf( stdout, STR(erf) "           : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(erfc) (fa);
  fprintf( stdout, STR(erfc) "          : " PRI "\n", f1);

  fa = 0.42;
  f1 = FUNC(exp) (fa);
  fprintf( stdout, STR(exp) "           : " PRI "\n", f1);

  fa = 0.42;
  f1 = FUNC(exp2) (fa);
  fprintf( stdout, STR(exp2) "          : " PRI "\n", f1);

  fa = 0.00042;
  f1 = FUNC(expm1) (fa);
  fprintf( stdout, STR(expm1) "         : " PRI "\n", f1);

  fa = -1.123;
  f1 = FUNC(fabs) (fa);
  fprintf( stdout, STR(fabs) "          : " PRI "\n", f1);

  fa = 1.123;
  fb = 1.123;
  f1 = FUNC(fdim) (fa, fb);
  fprintf( stdout, STR(fdim) "          : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(floor) (fa);
  fprintf( stdout, STR(floor) "         : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(floor) (fa);
  fprintf( stdout, STR(floor) "         : " PRI "\n", f1);

  fa = 2.1;
  fb = 2.2;
  fc = 3.01;
  f1 = FUNC(fma) (fa, fb, fc);
  fprintf( stdout, STR(fma) "           : " PRI "\n", f1);

  fa = -0.42;
  fb = 0.42;
  f1 = FUNC(fmax) (fa, fb);
  fprintf( stdout, STR(fmax) "          : " PRI "\n", f1);

  fa = -0.42;
  fb = 0.42;
  f1 = FUNC(fmin) (fa, fb);
  fprintf( stdout, STR(fmin) "          : " PRI "\n", f1);

  fa = 42.0;
  fb = 3.0;
  f1 = FUNC(fmod) (fa, fb);
  fprintf( stdout, STR(fmod) "          : " PRI "\n", f1);

  /* no type-specific variant */
  fa = 1.0;
  i1 = fpclassify(fa);
  fprintf( stdout, "fpclassify     : %d\n", i1);

  fa = 42.0;
  f1 = FUNC(frexp) (fa, &i1);
  fprintf( stdout, STR(frexp) "         : " PRI "\n", f1);

  fa = 42.0;
  fb = 42.0;
  f1 = FUNC(hypot) (fa, fb);
  fprintf( stdout, STR(hypot) "         : " PRI "\n", f1);

  fa = 42.0;
  i1 = FUNC(ilogb) (fa);
  fprintf( stdout, STR(ilogb) "         : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isfinite(fa);
  fprintf( stdout, "isfinite       : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isgreater(fa, fb);
  fprintf( stdout, "isgreater      : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isgreaterequal(fa, fb);
  fprintf( stdout, "isgreaterequal : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isinf(fa);
  fprintf( stdout, "isinf          : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isless(fa, fb);
  fprintf( stdout, "isless         : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = islessequal(fa, fb);
  fprintf( stdout, "islessequal    : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = islessgreater(fa, fb);
  fprintf( stdout, "islessgreater  : %d\n", i1);

  /* no type-specific variant */
  fa = 0.0;
  i1 = isnan(fa);
  fprintf( stdout, "isnan          : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isnormal(fa);
  fprintf( stdout, "isnormal       : %d\n", i1);

  /* no type-specific variant */
  fa = 1.0;
  fb = 2.0;
  f1 = isunordered(fa, fb);
  fprintf( stdout, "isunordered    : %d\n", i1);

  fa = 1.2;
  f1 = FUNC(j0) (fa);
  fprintf( stdout, STR(j0) "            : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(j1) (fa);
  fprintf( stdout, STR(j1) "            : " PRI "\n", f1);

  ia = 2;
  fa = 1.2;
  f1 = FUNC(jn) (ia, fa);
  fprintf( stdout, STR(jn) "            : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(ldexp) (fa, ia);
  fprintf( stdout, STR(ldexp) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(lgamma) (fa);
  fprintf( stdout, STR(lgamma) "        : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(llrint) (fa);
  fprintf( stdout, STR(llrint) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(llrint) (fa);
  fprintf( stdout, STR(llrint) "        : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(llround) (fa);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(llround) (fa);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log) (fa);
  fprintf( stdout, STR(log) "           : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log10) (fa);
  fprintf( stdout, STR(log10) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log1p) (fa);
  fprintf( stdout, STR(log1p) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log2) (fa);
  fprintf( stdout, STR(log2) "          : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(logb) (fa);
  fprintf( stdout, STR(logb) "          : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(lrint) (fa);
  fprintf( stdout, STR(lrint) "         : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(lrint) (fa);
  fprintf( stdout, STR(lrint) "         : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(lround) (fa);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(lround) (fa);
  fprintf( stdout, STR(lround) "        : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(modf) (fa, &f2);
  fprintf( stdout, STR(lmodf) "         : " PRI "\n", f1);

  f1 = FUNC(nan) ("");
  fprintf( stdout, STR(nan) "           : " PRI "\n", f1);

  fa = 1.5;
  f1 = FUNC(nearbyint) (fa);
  fprintf( stdout, STR(nearbyint) "     : " PRI "\n", f1);

  fa = 1.5;
  fb = 2.0;
  f1 = FUNC(nextafter) (fa, fb);
  fprintf( stdout, STR(nextafter) "     : " PRI "\n", f1);

  fa = 3.01;
  fb = 2.0;
  f1 = FUNC(pow) (fa, fb);
  fprintf( stdout, STR(pow) "           : " PRI "\n", f1);

  fa = 3.01;
  fb = 2.0;
  f1 = FUNC(remainder) (fa, fb);
  fprintf( stdout, STR(remainder) "     : " PRI "\n", f1);

  fa = 29.0;
  fb = 3.0;
  f1 = FUNC(remquo) (fa, fb, &i1);
  fprintf( stdout, STR(remquo) "        : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(rint) (fa);
  fprintf( stdout, STR(rint) "          : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(rint) (fa);
  fprintf( stdout, STR(rint) "          : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(round) (fa);
  fprintf( stdout, STR(round) "         : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(round) (fa);
  fprintf( stdout, STR(round) "         : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(scalbln) (fa, ia);
  fprintf( stdout, STR(scalbln) "       : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(scalbn) (fa, ia);
  fprintf( stdout, STR(scalbn) "        : " PRI "\n", f1);

  /* no type-specific variant */
  fa = 1.0;
  i1 = signbit(fa);
  fprintf( stdout, "signbit        : %i\n", i1);

  fa = M_PI_4;
  f1 = FUNC(sin) (fa);
  fprintf( stdout, STR(sin) "           : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(sinh) (fa);
  fprintf( stdout, STR(sinh) "          : " PRI "\n", f1);

  fa = 9.0;
  f1 = FUNC(sqrt) (fa);
  fprintf( stdout, STR(sqrt) "          : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(tan) (fa);
  fprintf( stdout, STR(tan) "           : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(tanh) (fa);
  fprintf( stdout, STR(tanh) "          : " PRI "\n", f1);

  fa = 2.1;
  f1 = FUNC(tgamma) (fa);
  fprintf( stdout, STR(tgamma) "        : " PRI "\n", f1);

  fa = 3.5;
  f1 = FUNC(trunc) (fa);
  fprintf( stdout, STR(trunc) "         : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(y0) (fa);
  fprintf( stdout, STR(y0) "            : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(y1) (fa);
  fprintf( stdout, STR(y1) "            : " PRI "\n", f1);

  ia = 3;
  fa = 1.2;
  f1 = FUNC(yn) (ia, fa);
  fprintf( stdout, STR(yn) "            : " PRI "\n", f1);
#endif
}
