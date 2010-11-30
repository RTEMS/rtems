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
domathl (void)
{
#ifndef NO_LONG_DOUBLE
  long double f1;
  long double f2;

  int i1;

  f1 = acosl(0.0);
  fprintf( stdout, "acosl          : %Lf\n", f1);

  f1 = acoshl(0.0);
  fprintf( stdout, "acoshl         : %Lf\n", f1);

  f1 = asinl(1.0);
  fprintf( stdout, "asinl          : %Lf\n", f1);

  f1 = asinhl(1.0);
  fprintf( stdout, "asinhl         : %Lf\n", f1);

  f1 = atanl(M_PI_4);
  fprintf( stdout, "atanl          : %Lf\n", f1);

  f1 = atan2l(2.3, 2.3);
  fprintf( stdout, "atan2l         : %Lf\n", f1);

  f1 = atanhl(1.0);
  fprintf( stdout, "atanhl         : %Lf\n", f1);

  f1 = cbrtl(27.0);
  fprintf( stdout, "cbrtl          : %Lf\n", f1);

  f1 = ceill(3.5);
  fprintf( stdout, "ceill          : %Lf\n", f1);

  f1 = copysignl(3.5, -2.5);
  fprintf( stdout, "copysignl      : %Lf\n", f1);

  f1 = cosl(M_PI_2);
  fprintf( stdout, "cosl           : %Lf\n", f1);

  f1 = coshl(M_PI_2);
  fprintf( stdout, "coshl          : %Lf\n", f1);

  f1 = erfl(42.0);
  fprintf( stdout, "erfl           : %Lf\n", f1);

  f1 = erfcl(42.0);
  fprintf( stdout, "erfcl          : %Lf\n", f1);

  f1 = expl(0.42);
  fprintf( stdout, "expl           : %Lf\n", f1);

  f1 = exp2l(0.42);
  fprintf( stdout, "exp2l          : %Lf\n", f1);

  f1 = expm1l(0.00042);
  fprintf( stdout, "expm1l         : %Lf\n", f1);

  f1 = fabsl(-1.123);
  fprintf( stdout, "fabsl          : %Lf\n", f1);

  f1 = fdiml(1.123, 2.123);
  fprintf( stdout, "fdiml          : %Lf\n", f1);

  f1 = floorl(0.5);
  fprintf( stdout, "floorl         : %Lf\n", f1);
  f1 = floorl(-0.5);
  fprintf( stdout, "floorl         : %Lf\n", f1);

  f1 = fmal(2.1, 2.2, 3.01);
  fprintf( stdout, "fmal           : %Lf\n", f1);

  f1 = fmaxl(-0.42, 0.42);
  fprintf( stdout, "fmaxl          : %Lf\n", f1);

  f1 = fminl(-0.42, 0.42);
  fprintf( stdout, "fminl          : %Lf\n", f1);

  f1 = fmodl(42.0, 3.0);
  fprintf( stdout, "fmodl          : %Lf\n", f1);

  /* no type-specific variant */
  i1 = fpclassify(1.0);
  fprintf( stdout, "fpclassify     : %d\n", i1);

  f1 = frexpl(42.0, &i1);
  fprintf( stdout, "frexpl         : %Lf\n", f1);

  f1 = hypotl(42.0, 42.0);
  fprintf( stdout, "hypotl         : %Lf\n", f1);

  i1 = ilogbl(42.0);
  fprintf( stdout, "ilogbl         : %d\n", i1);

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

  f1 = j0l(1.2);
  fprintf( stdout, "j0l            : %Lf\n", f1);

  f1 = j1l(1.2);
  fprintf( stdout, "j1l            : %Lf\n", f1);

  f1 = jnl(2,1.2);
  fprintf( stdout, "jnl            : %Lf\n", f1);

  f1 = ldexpl(1.2,3);
  fprintf( stdout, "ldexpl         : %Lf\n", f1);

  f1 = lgammal(42.0);
  fprintf( stdout, "lgammal        : %Lf\n", f1);

  f1 = llrintl(-0.5);
  fprintf( stdout, "llrintl        : %Lf\n", f1);
  f1 = llrintl(0.5);
  fprintf( stdout, "llrintl        : %Lf\n", f1);

  f1 = llroundl(-0.5);
  fprintf( stdout, "lroundl        : %Lf\n", f1);
  f1 = llroundl(0.5);
  fprintf( stdout, "lroundl        : %Lf\n", f1);

  f1 = logl(42.0);
  fprintf( stdout, "logl           : %Lf\n", f1);

  f1 = log10l(42.0);
  fprintf( stdout, "log10l         : %Lf\n", f1);

  f1 = log1pl(42.0);
  fprintf( stdout, "log1pl         : %Lf\n", f1);

  f1 = log2l(42.0);
  fprintf( stdout, "log2l          : %Lf\n", f1);

  f1 = logbl(42.0);
  fprintf( stdout, "logbl          : %Lf\n", f1);

  f1 = lrintl(-0.5);
  fprintf( stdout, "lrintl         : %Lf\n", f1);
  f1 = lrintl(0.5);
  fprintf( stdout, "lrintl         : %Lf\n", f1);

  f1 = lroundl(-0.5);
  fprintf( stdout, "lroundl        : %Lf\n", f1);
  f1 = lroundl(0.5);
  fprintf( stdout, "lroundl        : %Lf\n", f1);

  f1 = modfl(42.0,&f2);
  fprintf( stdout, "lmodfl         : %Lf\n", f1);

  f1 = nanl("");
  fprintf( stdout, "nanl           : %Lf\n", f1);

  f1 = nearbyintl(1.5);
  fprintf( stdout, "nearbyintl     : %Lf\n", f1);

  f1 = nextafterl(1.5,2.0);
  fprintf( stdout, "nextafterl     : %Lf\n", f1);

  f1 = powl(3.01, 2.0);
  fprintf( stdout, "powl           : %Lf\n", f1);

  f1 = remainderl(3.01,2.0);
  fprintf( stdout, "remainderl     : %Lf\n", f1);

  f1 = remquol(29.0,3.0,&i1);
  fprintf( stdout, "remquol        : %Lf\n", f1);

  f1 = rintl(0.5);
  fprintf( stdout, "rintl          : %Lf\n", f1);
  f1 = rintl(-0.5);
  fprintf( stdout, "rintl          : %Lf\n", f1);

  f1 = roundl(0.5);
  fprintf( stdout, "roundl         : %Lf\n", f1);
  f1 = roundl(-0.5);
  fprintf( stdout, "roundl         : %Lf\n", f1);

  f1 = scalblnl(1.2,3);
  fprintf( stdout, "scalblnl       : %Lf\n", f1);

  f1 = scalbnl(1.2,3);
  fprintf( stdout, "scalbnl        : %Lf\n", f1);

  /* no type-specific variant */
  i1 = signbit(1.0);
  fprintf( stdout, "signbit        : %i\n", i1);

  f1 = sinl(M_PI_4);
  fprintf( stdout, "sinl           : %Lf\n", f1);

  f1 = sinhl(M_PI_4);
  fprintf( stdout, "sinhl          : %Lf\n", f1);

  f1 = sqrtl(9.0);
  fprintf( stdout, "sqrtl          : %Lf\n", f1);

  f1 = tanl(M_PI_4);
  fprintf( stdout, "tanl           : %Lf\n", f1);

  f1 = tanhl(M_PI_4);
  fprintf( stdout, "tanhl          : %Lf\n", f1);

  f1 = tgammal(2.1);
  fprintf( stdout, "tanhl          : %Lf\n", f1);

  f1 = truncl(3.5);
  fprintf( stdout, "truncl         : %Lf\n", f1);

  f1 = y0l(1.2);
  fprintf( stdout, "y0l            : %Lf\n", f1);

  f1 = y1l(1.2);
  fprintf( stdout, "y1l            : %Lf\n", f1);

  f1 = ynl(3,1.2);
  fprintf( stdout, "ynl            : %Lf\n", f1);
#endif
}
