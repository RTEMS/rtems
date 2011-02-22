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

#include <sys/types.h>
#include <unistd.h>

int
main (void)
{
  off_t res;
  int fd = 42;

  res = lseek (fd, 0, SEEK_SET);
  res = lseek (fd, 1, SEEK_CUR);
  res = lseek (fd, 2, SEEK_END);

  return 0;
}
