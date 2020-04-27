/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Get Configurable System Variables
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/seterr.h>
#include <rtems/libio_.h>

#include <sys/param.h>

/*
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
)
{
  switch ( name ) {
    case _SC_CLK_TCK:
      return (long) rtems_clock_get_ticks_per_second();
    case _SC_OPEN_MAX:
      return rtems_libio_number_iops;
    case _SC_GETPW_R_SIZE_MAX:
      return 1024;
    case _SC_PAGESIZE:
      return PAGE_SIZE;
    case _SC_SYMLOOP_MAX:
      return RTEMS_FILESYSTEM_SYMLOOP_MAX;
    case _SC_NPROCESSORS_CONF:
      return (long) rtems_configuration_get_maximum_processors();
    case _SC_NPROCESSORS_ONLN:
      return (long) rtems_scheduler_get_processor_maximum();
    case _SC_POSIX_26_VERSION:
      return (long) _POSIX_26_VERSION;
#if defined(__sparc__)
    case 515: /* Solaris _SC_STACK_PROT */
      return 0;
#endif
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }
}
