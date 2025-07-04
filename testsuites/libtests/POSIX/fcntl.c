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

#include <unistd.h>
#include <fcntl.h>

int main (void)
{
  int rc;

  rc = fcntl (42, 43);
  (void) rc;
 
  rc = fcntl (42, 43, 44);

  return rc;
}
