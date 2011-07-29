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
  int ret;

  ret = iconv_close(cd);

  return 0;
}
