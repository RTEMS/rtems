/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>

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
      return _TOD_Ticks_per_second;

    case _SC_OPEN_MAX: {
        extern unsigned32 rtems_libio_number_iops;
        return rtems_libio_number_iops;
      }

    case _SC_GETPW_R_SIZE_MAX:
        return 1024;

#if defined(__sparc__)
    case 515: /* Solaris _SC_STACK_PROT */
     return 0;
#endif

    default:
      break;
  }

  errno = EINVAL;
  return -1;
}
