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
docomplexf (void)
{
#ifndef NO_FLOAT
  complex float ca, cb, cc;
  float f1;

  ca = 1.0 + 1.0 * I;
  cb = 1.0 - 1.0 * I;

  f1 = cabsf (ca);
  fprintf (stdout, "cabsf  : %f\n", f1);

  cc = cacosf (ca);
  fprintf (stdout, "cacosf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = cacoshf (ca);
  fprintf (stdout, "cacoshf: %f %fi\n", crealf (cc),
	   cimagf (cc));

  f1 = cargf (ca);
  fprintf (stdout, "cargf  : %f\n", f1);

  cc = casinf (ca);
  fprintf (stdout, "casinf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = casinhf (ca);
  fprintf (stdout, "casinhf: %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = catanf (ca);
  fprintf (stdout, "catanf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = catanhf (ca);
  fprintf (stdout, "catanhf: %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = ccosf (ca);
  fprintf (stdout, "ccosf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = ccoshf (ca);
  fprintf (stdout, "ccoshf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = cexpf (ca);
  fprintf (stdout, "cexpf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  f1 = cimagf (ca);
  fprintf (stdout, "cimagf : %f\n", f1);

  cc = clogf (ca);
  fprintf (stdout, "clogf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = conjf (ca);
  fprintf (stdout, "conjf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = cpowf (ca, cb);
  fprintf (stdout, "cpowf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = cprojf (ca);
  fprintf (stdout, "cprojf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  f1 = crealf (ca);
  fprintf (stdout, "crealf : %f\n", f1);

  cc = csinf (ca);
  fprintf (stdout, "csinf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = csinhf (ca);
  fprintf (stdout, "csinhf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = csqrtf (ca);
  fprintf (stdout, "csqrtf : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = ctanf (ca);
  fprintf (stdout, "ctanf  : %f %fi\n", crealf (cc),
	   cimagf (cc));

  cc = ctanhf (ca);
  fprintf (stdout, "ctanhf : %f %fi\n", crealf (cc),
	   cimagf (cc));
#endif
}
