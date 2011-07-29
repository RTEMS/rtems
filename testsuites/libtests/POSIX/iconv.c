/*
 * Copyright (c) 2011 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iconv.h>

int
main (void)
{
  iconv_t cd = NULL;
  char inbuf[42];
  size_t isize;
  char outbuf[42];
  size_t osize;
  size_t ret;

  char *i = inbuf;
  char *o = outbuf;
  ret = iconv(cd, &i, &isize, &o, &osize);

  return 0;
}
