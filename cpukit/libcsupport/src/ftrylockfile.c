/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FTRYLOCKFILE)

#include <stdio.h>
#include <rtems/seterr.h>
#include <errno.h>

/* This is a non-functional stub */
int ftrylockfile(FILE* file)
{
  (void) file;
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}

#endif
