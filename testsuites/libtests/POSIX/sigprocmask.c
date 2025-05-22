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

#include <signal.h>

int main(void)
{
  int rc;
  sigset_t set1 = { 0 };
  sigset_t set2 = { 0 };

  rc = sigprocmask(SIG_BLOCK, &set1, &set2);
  (void) rc;

  rc = sigprocmask(SIG_UNBLOCK, &set1, &set2);
  (void) rc;

  rc = sigprocmask(SIG_SETMASK, &set1, &set2);

  return rc;
}
