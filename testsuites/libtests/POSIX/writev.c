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

#include <sys/uio.h>

int main(void)
{
  struct iovec iov = { 0 };
  int count = 4;
  ssize_t ret;

  ret = writev(0, &iov, count);

  return ret;
}
