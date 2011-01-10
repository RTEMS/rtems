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
domathf (void)
{
#ifndef NO_FLOAT
  float f1;
  float f2;

  int i1;

  f1 = acosf(0.0);
  fprintf( stdout, "acosf          : %f\n", f1);

  f1 = acoshf(0.0);
  fprintf( stdout, "acoshf         : %f\n", f1);

  f1 = asinf(1.0);
  fprintf( stdout, "asinf          : %f\n", f1);

  f1 = asinhf(1.0);
  fprintf( stdout, "asinhf         : %f\n", f1);

  f1 = atanf(M_PI_4);
  fprintf( stdout, "atanf          : %f\n", f1);

  f1 = atan2f(2.3, 2.3);
  fprintf( stdout, "atan2f         : %f\n", f1);

  f1 = atanhf(1.0);
  fprintf( stdout, "atanhf         : %f\n", f1);

  f1 = cbrtf(27.0);
  fprintf( stdout, "cbrtf          : %f\n", f1);

  f1 = ceilf(3.5);
  fprintf( stdout, "ceilf          : %f\n", f1);

  f1 = copysignf(3.5, -2.5);
  fprintf( stdout, "copysignf      : %f\n", f1);

  f1 = cosf(M_PI_2);
  fprintf( stdout, "cosf           : %f\n", f1);

  f1 = coshf(M_PI_2);
  fprintf( stdout, "coshf          : %f\n", f1);

  f1 = erff(42.0);
  fprintf( stdout, "erff           : %f\n", f1);

  f1 = erfcf(42.0);
  fprintf( stdout, "erfcf          : %f\n", f1);

  f1 = expf(0.42);
  fprintf( stdout, "expf           : %f\n", f1);

  f1 = exp2f(0.42);
  fprintf( stdout, "exp2f          : %f\n", f1);

  f1 = expm1f(0.00042);
  fprintf( stdout, "expm1f         : %f\n", f1);

  f1 = fabsf(-1.123);
  fprintf( stdout, "fabsf          : %f\n", f1);

  f1 = fdimf(1.123, 2.123);
  fprintf( stdout, "fdimf          : %f\n", f1);

  f1 = floorf(0.5);
  fprintf( stdout, "floorf         : %f\n", f1);
  f1 = floorf(-0.5);
  fprintf( stdout, "floorf         : %f\n", f1);

  f1 = fmaf(2.1, 2.2, 3.01);
  fprintf( stdout, "fmaf           : %f\n", f1);

  f1 = fmaxf(-0.42, 0.42);
  fprintf( stdout, "fmaxf          : %f\n", f1);

  f1 = fminf(-0.42, 0.42);
  fprintf( stdout, "fminf          : %f\n", f1);

  f1 = fmodf(42.0, 3.0);
  fprintf( stdout, "fmodf          : %f\n", f1);

  /* no type-specific variant */
  i1 = fpclassify(1.0);
  fprintf( stdout, "fpclassify     : %d\n", i1);

  f1 = frexpf(42.0, &i1);
  fprintf( stdout, "frexpf         : %f\n", f1);

  f1 = hypotf(42.0, 42.0);
  fprintf( stdout, "hypotf         : %f\n", f1);

  i1 = ilogbf(42.0);
  fprintf( stdout, "ilogbf         : %d\n", i1);

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

  f1 = j0f(1.2);
  fprintf( stdout, "j0f            : %f\n", f1);

  f1 = j1f(1.2);
  fprintf( stdout, "j1f            : %f\n", f1);

  f1 = jnf(2,1.2);
  fprintf( stdout, "jnf            : %f\n", f1);

  f1 = ldexpf(1.2,3);
  fprintf( stdout, "ldexpf         : %f\n", f1);

  f1 = lgammaf(42.0);
  fprintf( stdout, "lgammaf        : %f\n", f1);

  f1 = llrintf(-0.5);
  fprintf( stdout, "llrintf        : %f\n", f1);
  f1 = llrintf(0.5);
  fprintf( stdout, "llrintf        : %f\n", f1);

  f1 = llroundf(-0.5);
  fprintf( stdout, "lroundf        : %f\n", f1);
  f1 = llroundf(0.5);
  fprintf( stdout, "lroundf        : %f\n", f1);

  f1 = logf(42.0);
  fprintf( stdout, "logf           : %f\n", f1);

  f1 = log10f(42.0);
  fprintf( stdout, "log10f         : %f\n", f1);

  f1 = log1pf(42.0);
  fprintf( stdout, "log1pf         : %f\n", f1);

  f1 = log2f(42.0);
  fprintf( stdout, "log2f          : %f\n", f1);

  f1 = logbf(42.0);
  fprintf( stdout, "logbf          : %f\n", f1);

  f1 = lrintf(-0.5);
  fprintf( stdout, "lrintf         : %f\n", f1);
  f1 = lrintf(0.5);
  fprintf( stdout, "lrintf         : %f\n", f1);

  f1 = lroundf(-0.5);
  fprintf( stdout, "lroundf        : %f\n", f1);
  f1 = lroundf(0.5);
  fprintf( stdout, "lroundf        : %f\n", f1);

  f1 = modff(42.0,&f2);
  fprintf( stdout, "lmodff         : %f\n", f1);

  f1 = nanf("");
  fprintf( stdout, "nanf           : %f\n", f1);

  f1 = nearbyintf(1.5);
  fprintf( stdout, "nearbyintf     : %f\n", f1);

  f1 = nextafterf(1.5,2.0);
  fprintf( stdout, "nextafterf     : %f\n", f1);

  f1 = powf(3.01, 2.0);
  fprintf( stdout, "powf           : %f\n", f1);

  f1 = remainderf(3.01,2.0);
  fprintf( stdout, "remainderf     : %f\n", f1);

  f1 = remquof(29.0,3.0,&i1);
  fprintf( stdout, "remquof        : %f\n", f1);

  f1 = rintf(0.5);
  fprintf( stdout, "rintf          : %f\n", f1);
  f1 = rintf(-0.5);
  fprintf( stdout, "rintf          : %f\n", f1);

  f1 = roundf(0.5);
  fprintf( stdout, "roundf         : %f\n", f1);
  f1 = roundf(-0.5);
  fprintf( stdout, "roundf         : %f\n", f1);

  f1 = scalblnf(1.2,3);
  fprintf( stdout, "scalblnf       : %f\n", f1);

  f1 = scalbnf(1.2,3);
  fprintf( stdout, "scalbnf        : %f\n", f1);

  /* no type-specific variant */
  i1 = signbit(1.0);
  fprintf( stdout, "signbit        : %i\n", i1);

  f1 = sinf(M_PI_4);
  fprintf( stdout, "sinf           : %f\n", f1);

  f1 = sinhf(M_PI_4);
  fprintf( stdout, "sinhf          : %f\n", f1);

  f1 = sqrtf(9.0);
  fprintf( stdout, "sqrtf          : %f\n", f1);

  f1 = tanf(M_PI_4);
  fprintf( stdout, "tanf           : %f\n", f1);

  f1 = tanhf(M_PI_4);
  fprintf( stdout, "tanhf          : %f\n", f1);

  f1 = tgammaf(2.1);
  fprintf( stdout, "tgammaf        : %f\n", f1);

  f1 = truncf(3.5);
  fprintf( stdout, "truncf         : %f\n", f1);

  f1 = y0f(1.2);
  fprintf( stdout, "y0f            : %f\n", f1);

  f1 = y1f(1.2);
  fprintf( stdout, "y1f            : %f\n", f1);

  f1 = ynf(3,1.2);
  fprintf( stdout, "ynf            : %f\n", f1);
#endif
}
