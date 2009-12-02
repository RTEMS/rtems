/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <unistd.h>

int
main (void)
{
  int fd = 42;
  char buf[4];
  ssize_t len;

  len = read (fd, &buf, 4);

  return 0;
}
