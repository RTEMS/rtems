/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <unistd.h>

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

o   case _SC_OPEN_MAX: {
        extern unsigned32 rtems_libio_number_iops;
        return rtems_libio_number_iops;
      }

#if defined(__sparc__)
    case 515: /* Solaris _SC_STACK_PROT */
     return 0;
#endif

    default:
      break;
  }

  return POSIX_NOT_IMPLEMENTED();
}
