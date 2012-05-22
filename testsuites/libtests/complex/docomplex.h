/*
 * Copyright (c) 2010, 2011 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <complex.h>
#include <stdio.h>

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

extern void FUNC(docomplex) (void);

void
FUNC(docomplex) (void)
{
#ifndef PROVIDE_EMPTY_FUNC
  complex FTYPE ca, cb, cc;
  FTYPE f1;

  ca = 1.0 + 1.0 * I;
  cb = 1.0 - 1.0 * I;

  f1 = FUNC(cabs) (ca);
  fprintf (stdout, STR(cabs) "  : " PRI "\n", f1);

  cc = FUNC(cacos) (ca);
  fprintf (stdout, STR(cacos) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cacosh) (ca);
  fprintf (stdout, STR(cacosh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(carg) (ca);
  fprintf (stdout, STR(carg) "  : " PRI "\n", f1);

  cc = FUNC(casin) (ca);
  fprintf (stdout, STR(casin) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(casinh) (ca);
  fprintf (stdout, STR(casinh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(catan) (ca);
  fprintf (stdout, STR(catan) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(catanh) (ca);
  fprintf (stdout, STR(catanh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ccos) (ca);
  fprintf (stdout, STR(ccos) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ccosh) (ca);
  fprintf (stdout, STR(ccosh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cexp) (ca);
  fprintf (stdout, STR(cexp) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(cimag) (ca);
  fprintf (stdout, STR(cimag) " : " PRI "\n", f1);

  cc = FUNC(clog) (ca);
  fprintf (stdout, STR(clog) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(conj) (ca);
  fprintf (stdout, STR(conj) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cpow) (ca, cb);
  fprintf (stdout, STR(cpow) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cproj) (ca);
  fprintf (stdout, STR(cproj) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(creal) (ca);
  fprintf (stdout, STR(creal) " : " PRI "\n", f1);

  cc = FUNC(csin) (ca);
  fprintf (stdout, STR(csin) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(csinh) (ca);
  fprintf (stdout, STR(csinh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(csqrt) (ca);
  fprintf (stdout, STR(csqrt) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ctan) (ca);
  fprintf (stdout, STR(ctan) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ctanh) (ca);
  fprintf (stdout, STR(ctanh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));
#endif
}
