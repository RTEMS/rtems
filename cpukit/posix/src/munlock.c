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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>
#include <errno.h>
#include <rtems/seterr.h>

int munlock( const void *addr, size_t len )
{
  (void) addr;
  (void) len;

  rtems_set_errno_and_return_minus_one( ENOMEM );
}
