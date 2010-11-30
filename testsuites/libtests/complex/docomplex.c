/*
 * Copyright (c) 2010 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/*
 * Try to compile and link against POSIX complex math routines.
 */

#include <complex.h>
#include <stdio.h>

void
docomplex  (void)
{
#ifndef NO_DOUBLE
  complex double ca, cb, cc;
  double f1;

  ca = 1.0 + 1.0 * I;
  cb = 1.0 - 1.0 * I;

  f1 = cabs  (ca);
  fprintf (stdout, "cabs   : %f\n", f1);

  cc = cacos  (ca);
  fprintf (stdout, "cacos  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = cacosh  (ca);
  fprintf (stdout, "cacosh : %f %fi\n", creal  (cc),
	   cimag  (cc));

  f1 = carg  (ca);
  fprintf (stdout, "carg   : %f\n", f1);

  cc = casin  (ca);
  fprintf (stdout, "casin  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = casinh  (ca);
  fprintf (stdout, "casinh : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = catan  (ca);
  fprintf (stdout, "catan  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = catanh  (ca);
  fprintf (stdout, "catanh : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = ccos  (ca);
  fprintf (stdout, "ccos   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = ccosh  (ca);
  fprintf (stdout, "ccosh  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = cexp  (ca);
  fprintf (stdout, "cexp   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  f1 = cimag  (ca);
  fprintf (stdout, "cimag  : %f\n", f1);

  cc = clog  (ca);
  fprintf (stdout, "clog   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = conj  (ca);
  fprintf (stdout, "conj   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = cpow  (ca, cb);
  fprintf (stdout, "cpow   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = cproj  (ca);
  fprintf (stdout, "cproj  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  f1 = creal  (ca);
  fprintf (stdout, "creal  : %f\n", f1);

  cc = csin  (ca);
  fprintf (stdout, "csin   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = csinh  (ca);
  fprintf (stdout, "csinh  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = csqrt  (ca);
  fprintf (stdout, "csqrt  : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = ctan  (ca);
  fprintf (stdout, "ctan   : %f %fi\n", creal  (cc),
	   cimag  (cc));

  cc = ctanh  (ca);
  fprintf (stdout, "ctanh  : %f %fi\n", creal  (cc),
	   cimag  (cc));
#endif
}
