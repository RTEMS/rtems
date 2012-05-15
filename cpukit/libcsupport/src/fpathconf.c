/*
 *  fpathconf() - POSIX 1003.1b - 5.7.1 - Configurable Pathname Varables
 *
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/seterr.h>

#include <unistd.h>
#include <errno.h>

long fpathconf(
  int   fd,
  int   name
)
{
  long                                    return_value;
  rtems_libio_t                          *iop;
  const rtems_filesystem_limits_and_options_t *the_limits;

  rtems_libio_check_fd(fd);
  iop = rtems_libio_iop(fd);
  rtems_libio_check_is_open(iop);

  /*
   *  Now process the information request.
   */

  the_limits = iop->pathinfo.mt_entry->pathconf_limits_and_options;

  switch ( name ) {
    case _PC_LINK_MAX:
      return_value = the_limits->link_max;
      break;
    case _PC_MAX_CANON:
      return_value = the_limits->max_canon;
      break;
    case _PC_MAX_INPUT:
      return_value = the_limits->max_input;
      break;
    case _PC_NAME_MAX:
      return_value = the_limits->name_max;
      break;
    case _PC_PATH_MAX:
      return_value = the_limits->path_max;
      break;
    case _PC_PIPE_BUF:
      return_value = the_limits->pipe_buf;
      break;
    case _PC_CHOWN_RESTRICTED:
      return_value = the_limits->posix_chown_restrictions;
      break;
    case _PC_NO_TRUNC:
      return_value = the_limits->posix_no_trunc;
      break;
    case _PC_VDISABLE:
      return_value = the_limits->posix_vdisable;
      break;
    case _PC_ASYNC_IO:
      return_value = the_limits->posix_async_io;
      break;
    case _PC_PRIO_IO:
      return_value = the_limits->posix_prio_io;
      break;
    case _PC_SYNC_IO:
      return_value = the_limits->posix_sync_io;
      break;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
      break;
  }

  return return_value;
}
