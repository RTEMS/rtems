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

static volatile complex FTYPE ca, cb;

void
FUNC(docomplex) (void)
{
#ifndef PROVIDE_EMPTY_FUNC
  complex FTYPE cc;
  FTYPE f1;

  ca = 1.0 + 1.0 * I;
  cb = 1.0 - 1.0 * I;

  f1 = FUNC(cabs) (ca);
  printf (STR(cabs) "  : " PRI "\n", f1);

  cc = FUNC(cacos) (ca);
  printf (STR(cacos) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cacosh) (ca);
  printf (STR(cacosh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(carg) (ca);
  printf (STR(carg) "  : " PRI "\n", f1);

  cc = FUNC(casin) (ca);
  printf (STR(casin) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(casinh) (ca);
  printf (STR(casinh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(catan) (ca);
  printf (STR(catan) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(catanh) (ca);
  printf (STR(catanh) ": " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ccos) (ca);
  printf (STR(ccos) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ccosh) (ca);
  printf (STR(ccosh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cexp) (ca);
  printf (STR(cexp) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(cimag) (ca);
  printf (STR(cimag) " : " PRI "\n", f1);

  cc = FUNC(clog) (ca);
  printf (STR(clog) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(conj) (ca);
  printf (STR(conj) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cpow) (ca, cb);
  printf (STR(cpow) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(cproj) (ca);
  printf (STR(cproj) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  f1 = FUNC(creal) (ca);
  printf (STR(creal) " : " PRI "\n", f1);

  cc = FUNC(csin) (ca);
  printf (STR(csin) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(csinh) (ca);
  printf (STR(csinh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(csqrt) (ca);
  printf (STR(csqrt) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ctan) (ca);
  printf (STR(ctan) "  : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));

  cc = FUNC(ctanh) (ca);
  printf (STR(ctanh) " : " PRI " " PRI "i\n", FUNC(creal) (cc),
	   FUNC(cimag) (cc));
#endif
}
