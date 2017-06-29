/**
 * @file
 *
 * @brief Default MMAP Handler
 *
 * @ingroup LibIOFSHandler
 */

/*
 * Copyright (c) 2017 Kevin Kirspel (kirspkt@gmail.com)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

int rtems_filesystem_default_mmap(
  rtems_libio_t  *iop,
  void          **addr,
  size_t          len,
  int             prot,
  off_t           off
)
{
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}

int rtems_termios_mmap(
  rtems_libio_t  *iop,
  void          **addr,
  size_t          len,
  int             prot,
  off_t           off
) RTEMS_WEAK_ALIAS( rtems_filesystem_default_mmap );
