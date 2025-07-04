/*
 * Copyright (c) 2009 by
 * Ralf Corsepius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

int
main (void)
{
  void *foo = calloc (42, 43);
  int rc = (foo != NULL);
  free(foo);

  return rc;
}
