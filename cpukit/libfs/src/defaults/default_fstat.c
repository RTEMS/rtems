/*
 *  COPYRIGHT (c) 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

int rtems_filesystem_default_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  buf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;

  return 0;
}
