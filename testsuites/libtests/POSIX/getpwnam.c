/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <sys/types.h>
#include <pwd.h>

int
main (void)
{
  struct passwd *pass;
  pass = getpwnam ("root");

  return 0;
}
