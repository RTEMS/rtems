/**
 *  @file
 *
 *  @brief Set File Creation Mask
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>

#include <rtems/libio_.h>

/**
 *  POSIX 1003.1b 5.3.3 - Set File Creation Mask
 */
mode_t umask( mode_t cmask )
{
  mode_t old_mask;

  rtems_libio_lock();
  old_mask = rtems_filesystem_umask;
  rtems_filesystem_umask = cmask & (S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_libio_unlock();

  return old_mask;
}
