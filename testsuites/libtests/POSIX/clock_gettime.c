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

#include <time.h>

int
main (void)
{
  struct timespec mytime;

  return clock_gettime(CLOCK_REALTIME,&mytime);
}
