/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/tod.h>

#include <sys/param.h>

/*PAGE
 *
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
)
{

  switch (name) {
    case _SC_CLK_TCK:
      return (TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick);

    case _SC_OPEN_MAX: {
        extern uint32_t   rtems_libio_number_iops;
        return rtems_libio_number_iops;
      }

    case _SC_GETPW_R_SIZE_MAX:
        return 1024;
    
    case _SC_PAGESIZE:
        return PAGE_SIZE;

#if defined(__sparc__)
    case 515: /* Solaris _SC_STACK_PROT */
     return 0;
#endif

    default:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
