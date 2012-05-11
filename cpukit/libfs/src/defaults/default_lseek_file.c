/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

off_t rtems_filesystem_default_lseek_file(
  rtems_libio_t *iop,
  off_t offset,
  int whence
)
{
  off_t rv = 0;
  off_t reference_offset;
  off_t new_offset;
  struct stat st;

  switch ( whence ) {
    case SEEK_SET:
      reference_offset = 0;
      break;
    case SEEK_CUR:
      reference_offset = iop->offset;
      break;
    case SEEK_END:
      st.st_size = 0;
      rv = (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, &st );
      reference_offset = st.st_size;
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }
  new_offset = reference_offset + offset;

  if ( rv == 0 ) {
    if (
      (offset >= 0 && new_offset >= reference_offset)
        || (offset < 0 && new_offset < reference_offset)
    ) {
      if ( new_offset >= 0 ) {
        iop->offset = new_offset;
        rv = new_offset;
      } else {
        errno = EINVAL;
        rv = -1;
      }
    } else {
      errno = EOVERFLOW;
      rv = -1;
    }
  }

  return rv;
}
