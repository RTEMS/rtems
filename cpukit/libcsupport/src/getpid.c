/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getpid( void )
{
  return _Objects_Local_node;
}

/*
 *  _getpid_r
 *
 *  This is the Newlib dependent reentrant version of getpid().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

pid_t _getpid_r(
  struct _reent *ptr
)
{
  return getpid();
}
#endif

