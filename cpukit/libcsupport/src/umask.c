/*
 *  umask() - POSIX 1003.1b 5.3.3 - Set File Creation Mask
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/libio_.h>

mode_t umask(
  mode_t cmask
)
{
  mode_t old_mask;

  old_mask               = rtems_filesystem_umask;
  rtems_filesystem_umask = cmask;

  return old_mask;
}
