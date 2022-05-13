/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of sync().
 */

/*
 * Copyright (C) 2022 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

void sync( void )
{
  int fd;

  for ( fd = 0; fd < (int) rtems_libio_number_iops; ++fd ) {
    (void) fsync( fd );
    (void) fdatasync( fd );
  }
}
