/*
 *  fchdir() - compatible with SVr4, 4.4BSD and X/OPEN - Change Directory
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

int fchdir(
  int       fd
)
{
  rtems_libio_t *iop;
  
  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Now process the fchmod().
   */

  rtems_libio_check_permissions( iop, LIBIO_FLAGS_READ );

  /*
   * Verify you can change directory into this node.
   */

  if ( !iop->pathinfo.ops ) {
    set_errno_and_return_minus_one( ENOTSUP );
  }

  if ( !iop->pathinfo.ops->node_type_h ) {
    set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*iop->pathinfo.ops->node_type_h)( &iop->pathinfo ) !=
                                          RTEMS_FILESYSTEM_DIRECTORY ) {
    set_errno_and_return_minus_one( ENOTDIR );
  }
  
  rtems_filesystem_freenode( &rtems_filesystem_current );

  /*
   * FIXME : I feel there should be another call to
   *         actually take into account the extra reference to
   *         this node which we are making here. I can
   *         see the freenode interface but do not see
   *         allocnode node interface. It maybe node_type.
   */
  
  rtems_filesystem_current = iop->pathinfo;
  
  return 0;
}

