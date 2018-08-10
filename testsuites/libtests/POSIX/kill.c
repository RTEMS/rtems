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

int main(void)
{
  pid_t pid = 0;
  int rc;

  rc = kill(pid, SIGHUP);
  (void) rc;

  rc = kill(pid, SIGKILL);
  (void) rc;

  rc = kill(pid, SIGTERM);

  return rc;
}
