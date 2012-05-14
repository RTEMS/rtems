/*
 *  fchown() - POSIX 1003.1b 5.6.5 - Change Owner and Group of a File
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

#include <unistd.h>

#include <rtems/libio_.h>

int fchown( int fd, uid_t owner, gid_t group )
{
  int rv = 0;
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  if (iop->pathinfo.mt_entry->writeable) {
    rtems_filesystem_instance_lock( &iop->pathinfo );
    rv = (*iop->pathinfo.mt_entry->ops->chown_h)(
      &iop->pathinfo,
      owner,
      group
    );
    rtems_filesystem_instance_unlock( &iop->pathinfo );
  } else {
    errno = EROFS;
    rv = -1;
  }

  return rv;
}
