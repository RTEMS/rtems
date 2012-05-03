/*
 *  umask() - POSIX 1003.1b 5.3.3 - Set File Creation Mask
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <sys/stat.h>

#include <rtems/libio_.h>

mode_t umask( mode_t cmask )
{
  mode_t old_mask;

  /*
   * We must use the same protection mechanism as in
   * rtems_libio_set_private_env().
   */
  _Thread_Disable_dispatch();
  old_mask = rtems_filesystem_umask;
  rtems_filesystem_umask = cmask & (S_IRWXU | S_IRWXG | S_IRWXO);
  _Thread_Enable_dispatch();

  return old_mask;
}
