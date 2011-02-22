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
#include <sys/stat.h>
#include <fcntl.h>

int
main (void)
{
  int fd1;
  int fd2;

  fd1 = open ("/tmp/foo1", O_RDWR | O_APPEND);
  fd2 = open ("/tmp/foo2", O_CREAT, S_IWUSR);

  return 0;
}
