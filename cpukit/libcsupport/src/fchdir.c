/*
 *  fchdir() - compatible with SVr4, 4.4BSD and X/OPEN - Change Directory
 *
 *  COPYRIGHT (c) 1989-2000.
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

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int fchdir(
  int       fd
)
{
  rtems_libio_t *iop;
  rtems_filesystem_location_info_t loc, saved;
  
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
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if ( !iop->pathinfo.ops->node_type_h ) {
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*iop->pathinfo.ops->node_type_h)( &iop->pathinfo ) !=
                                          RTEMS_FILESYSTEM_DIRECTORY ) {
    rtems_set_errno_and_return_minus_one( ENOTDIR );
  }
  

  /*
   * FIXME : I feel there should be another call to
   *         actually take into account the extra reference to
   *         this node which we are making here. I can
   *         see the freenode interface but do not see
   *         allocnode node interface. It maybe node_type.
   *
   * FIXED:  T.Straumann: it is evaluate_path()
   *         but note the race condition. Threads who
   *         share their rtems_filesystem_current better
   *         be synchronized!
   */
  
  saved                    = rtems_filesystem_current;
  rtems_filesystem_current = iop->pathinfo;
  
  /* clone the current node */
  if (rtems_filesystem_evaluate_path(".", 0, &loc, 0)) {
    /* cloning failed; restore original and bail out */
    rtems_filesystem_current = saved;
	return -1;
  }
  /* release the old one */
  rtems_filesystem_freenode( &saved );

  rtems_filesystem_current = loc;
  
  return 0;
}
