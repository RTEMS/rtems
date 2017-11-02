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
  printf( STR(acos) "          : " PRI "\n", f1);

  fa = 0.0;
  f1 = FUNC(acosh) (fa);
  printf( STR(acosh) "         : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(asin) (fa);
  printf( STR(asin) "          : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(asinh) (fa);
  printf( STR(asinh) "         : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(atan) (fa);
  printf( STR(atan) "          : " PRI "\n", f1);

  fa = 2.3;
  fb = 2.3;
  f1 = FUNC(atan2) (fa, fb);
  printf( STR(atan2) "         : " PRI "\n", f1);

  fa = 1.0;
  f1 = FUNC(atanh) (fa);
  printf( STR(atanh) "         : " PRI "\n", f1);

  fa = 27.0;
  f1 = FUNC(cbrt) (fa);
  printf( STR(cbrt) "          : " PRI "\n", f1);

  fa = 3.5;
  f1 = FUNC(ceil) (fa);
  printf( STR(ceil) "          : " PRI "\n", f1);

  fa = 3.5;
  fb = -2.5;
  f1 = FUNC(copysign) (fa, fb);
  printf( STR(copysign) "      : " PRI "\n", f1);

  fa = M_PI_2;
  f1 = FUNC(cos) (fa);
  printf( STR(cos) "           : " PRI "\n", f1);

  fa = M_PI_2;
  f1 = FUNC(cosh) (fa);
  printf( STR(cosh) "          : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(erf) (fa);
  printf( STR(erf) "           : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(erfc) (fa);
  printf( STR(erfc) "          : " PRI "\n", f1);

  fa = 0.42;
  f1 = FUNC(exp) (fa);
  printf( STR(exp) "           : " PRI "\n", f1);

  fa = 0.42;
  f1 = FUNC(exp2) (fa);
  printf( STR(exp2) "          : " PRI "\n", f1);

  fa = 0.00042;
  f1 = FUNC(expm1) (fa);
  printf( STR(expm1) "         : " PRI "\n", f1);

  fa = -1.123;
  f1 = FUNC(fabs) (fa);
  printf( STR(fabs) "          : " PRI "\n", f1);

  fa = 1.123;
  fb = 1.123;
  f1 = FUNC(fdim) (fa, fb);
  printf( STR(fdim) "          : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(floor) (fa);
  printf( STR(floor) "         : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(floor) (fa);
  printf( STR(floor) "         : " PRI "\n", f1);

  fa = 2.1;
  fb = 2.2;
  fc = 3.01;
  f1 = FUNC(fma) (fa, fb, fc);
  printf( STR(fma) "           : " PRI "\n", f1);

  fa = -0.42;
  fb = 0.42;
  f1 = FUNC(fmax) (fa, fb);
  printf( STR(fmax) "          : " PRI "\n", f1);

  fa = -0.42;
  fb = 0.42;
  f1 = FUNC(fmin) (fa, fb);
  printf( STR(fmin) "          : " PRI "\n", f1);

  fa = 42.0;
  fb = 3.0;
  f1 = FUNC(fmod) (fa, fb);
  printf( STR(fmod) "          : " PRI "\n", f1);

  /* no type-specific variant */
  fa = 1.0;
  i1 = fpclassify(fa);
  printf( "fpclassify     : %d\n", i1);

  fa = 42.0;
  f1 = FUNC(frexp) (fa, &i1);
  printf( STR(frexp) "         : " PRI "\n", f1);

  fa = 42.0;
  fb = 42.0;
  f1 = FUNC(hypot) (fa, fb);
  printf( STR(hypot) "         : " PRI "\n", f1);

  fa = 42.0;
  i1 = FUNC(ilogb) (fa);
  printf( STR(ilogb) "         : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isfinite(fa);
  printf( "isfinite       : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isgreater(fa, fb);
  printf( "isgreater      : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isgreaterequal(fa, fb);
  printf( "isgreaterequal : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isinf(fa);
  printf( "isinf          : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = isless(fa, fb);
  printf( "isless         : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = islessequal(fa, fb);
  printf( "islessequal    : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  fb = 3.1;
  i1 = islessgreater(fa, fb);
  printf( "islessgreater  : %d\n", i1);

  /* no type-specific variant */
  fa = 0.0;
  i1 = isnan(fa);
  printf( "isnan          : %d\n", i1);

  /* no type-specific variant */
  fa = 3.0;
  i1 = isnormal(fa);
  printf( "isnormal       : %d\n", i1);

  /* no type-specific variant */
  fa = 1.0;
  fb = 2.0;
  f1 = isunordered(fa, fb);
  printf( "isunordered    : %d\n", i1);

  fa = 1.2;
  f1 = FUNC(j0) (fa);
  printf( STR(j0) "            : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(j1) (fa);
  printf( STR(j1) "            : " PRI "\n", f1);

  ia = 2;
  fa = 1.2;
  f1 = FUNC(jn) (ia, fa);
  printf( STR(jn) "            : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(ldexp) (fa, ia);
  printf( STR(ldexp) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(lgamma) (fa);
  printf( STR(lgamma) "        : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(llrint) (fa);
  printf( STR(llrint) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(llrint) (fa);
  printf( STR(llrint) "        : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(llround) (fa);
  printf( STR(lround) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(llround) (fa);
  printf( STR(lround) "        : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log) (fa);
  printf( STR(log) "           : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log10) (fa);
  printf( STR(log10) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log1p) (fa);
  printf( STR(log1p) "         : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(log2) (fa);
  printf( STR(log2) "          : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(logb) (fa);
  printf( STR(logb) "          : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(lrint) (fa);
  printf( STR(lrint) "         : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(lrint) (fa);
  printf( STR(lrint) "         : " PRI "\n", f1);

  fa = -0.5;
  f1 = FUNC(lround) (fa);
  printf( STR(lround) "        : " PRI "\n", f1);
  fa = 0.5;
  f1 = FUNC(lround) (fa);
  printf( STR(lround) "        : " PRI "\n", f1);

  fa = 42.0;
  f1 = FUNC(modf) (fa, &f2);
  printf( STR(lmodf) "         : " PRI "\n", f1);

  f1 = FUNC(nan) ("");
  printf( STR(nan) "           : " PRI "\n", f1);

  fa = 1.5;
  f1 = FUNC(nearbyint) (fa);
  printf( STR(nearbyint) "     : " PRI "\n", f1);

  fa = 1.5;
  fb = 2.0;
  f1 = FUNC(nextafter) (fa, fb);
  printf( STR(nextafter) "     : " PRI "\n", f1);

  fa = 3.01;
  fb = 2.0;
  f1 = FUNC(pow) (fa, fb);
  printf( STR(pow) "           : " PRI "\n", f1);

  fa = 3.01;
  fb = 2.0;
  f1 = FUNC(remainder) (fa, fb);
  printf( STR(remainder) "     : " PRI "\n", f1);

  fa = 29.0;
  fb = 3.0;
  f1 = FUNC(remquo) (fa, fb, &i1);
  printf( STR(remquo) "        : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(rint) (fa);
  printf( STR(rint) "          : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(rint) (fa);
  printf( STR(rint) "          : " PRI "\n", f1);

  fa = 0.5;
  f1 = FUNC(round) (fa);
  printf( STR(round) "         : " PRI "\n", f1);
  fa = -0.5;
  f1 = FUNC(round) (fa);
  printf( STR(round) "         : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(scalbln) (fa, ia);
  printf( STR(scalbln) "       : " PRI "\n", f1);

  fa = 1.2;
  ia = 3;
  f1 = FUNC(scalbn) (fa, ia);
  printf( STR(scalbn) "        : " PRI "\n", f1);

  /* no type-specific variant */
  fa = 1.0;
  i1 = signbit(fa);
  printf( "signbit        : %i\n", i1);

  fa = M_PI_4;
  f1 = FUNC(sin) (fa);
  printf( STR(sin) "           : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(sinh) (fa);
  printf( STR(sinh) "          : " PRI "\n", f1);

  fa = 9.0;
  f1 = FUNC(sqrt) (fa);
  printf( STR(sqrt) "          : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(tan) (fa);
  printf( STR(tan) "           : " PRI "\n", f1);

  fa = M_PI_4;
  f1 = FUNC(tanh) (fa);
  printf( STR(tanh) "          : " PRI "\n", f1);

  fa = 2.1;
  f1 = FUNC(tgamma) (fa);
  printf( STR(tgamma) "        : " PRI "\n", f1);

  fa = 3.5;
  f1 = FUNC(trunc) (fa);
  printf( STR(trunc) "         : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(y0) (fa);
  printf( STR(y0) "            : " PRI "\n", f1);

  fa = 1.2;
  f1 = FUNC(y1) (fa);
  printf( STR(y1) "            : " PRI "\n", f1);

  ia = 3;
  fa = 1.2;
  f1 = FUNC(yn) (ia, fa);
  printf( STR(yn) "            : " PRI "\n", f1);
#endif
}
