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

int main(void)
{
  off_t res;
  int fd = 42;

  res = lseek(fd, 0, SEEK_SET);
  (void) res;

  res = lseek(fd, 1, SEEK_CUR);
  (void) res;

  res = lseek(fd, 2, SEEK_END);
  (void) res;

  return 0;
}
