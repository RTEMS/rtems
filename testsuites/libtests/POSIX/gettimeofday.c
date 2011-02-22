/*
 * Copyright (c) 2011 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>

int
main (void)
{
  struct timeval tv;
  struct timezone tz;

  return gettimeofday(&tv, &tz);
}
