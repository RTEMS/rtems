/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>
#include <errno.h>
#include <rtems/seterr.h>

int posix_madvise( void *addr, size_t len, int advice )
{
  (void) addr;
  (void) len;
  (void) advice;

  return EINVAL;
}
