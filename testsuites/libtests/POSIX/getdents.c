/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <dirent.h>

int
main (void)
{
  int status;
  int fd = 42;

  status = getdents (fd, "/tmp/foo", 0);
}
