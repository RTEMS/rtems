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
docomplexl (void)
{
#ifndef NO_LONG_DOUBLE
  complex long double ca, cb, cc;
  long double f1;

  ca = 1.0 + 1.0 * I;
  cb = 1.0 - 1.0 * I;

  f1 = cabsl (ca);
  fprintf (stdout, "cabsl  : %Lf\n", f1);

  cc = cacosl (ca);
  fprintf (stdout, "cacosl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = cacoshl (ca);
  fprintf (stdout, "cacoshl: %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  f1 = cargl (ca);
  fprintf (stdout, "cargl  : %Lf\n", f1);

  cc = casinl (ca);
  fprintf (stdout, "casinl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = casinhl (ca);
  fprintf (stdout, "casinhl: %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = catanl (ca);
  fprintf (stdout, "catanl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = catanhl (ca);
  fprintf (stdout, "catanhl: %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = ccosl (ca);
  fprintf (stdout, "ccosl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = ccoshl (ca);
  fprintf (stdout, "ccoshl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = cexpl (ca);
  fprintf (stdout, "cexpl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  f1 = cimagl (ca);
  fprintf (stdout, "cimagl : %Lf\n", f1);

  cc = clogl (ca);
  fprintf (stdout, "clogl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = conjl (ca);
  fprintf (stdout, "conjl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = cpowl (ca, cb);
  fprintf (stdout, "cpowl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = cprojl (ca);
  fprintf (stdout, "cprojl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  f1 = creall (ca);
  fprintf (stdout, "creall : %Lf\n", f1);

  cc = csinl (ca);
  fprintf (stdout, "csinl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = csinhl (ca);
  fprintf (stdout, "csinhl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = csqrtl (ca);
  fprintf (stdout, "csqrtl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = ctanl (ca);
  fprintf (stdout, "ctanl  : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));

  cc = ctanhl (ca);
  fprintf (stdout, "ctanhl : %Lf %Lfi\n", creall (cc),
	   cimagl (cc));
#endif
}
