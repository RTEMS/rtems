/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

int
main (void)
{
  struct timespec req = { 0, 42 };
  struct timespec rem;
  int status;

  status = nanosleep (&req, &rem);

  return 0;
}
