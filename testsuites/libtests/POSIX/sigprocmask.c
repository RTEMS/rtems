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

#include <signal.h>

int
main (void)
{
  int status;
  sigset_t set1, set2;

  status = sigprocmask (SIG_BLOCK, &set1, &set2);
  status = sigprocmask (SIG_UNBLOCK, &set1, &set2);
  status = sigprocmask (SIG_SETMASK, &set1, &set2);

  return 0;
}
