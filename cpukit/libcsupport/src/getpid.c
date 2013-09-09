/**
 *  @file
 *
 *  @brief Process and Parent Process IDs
 *  @ingroup libcsupport
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/score/objectimpl.h>
#include <rtems/seterr.h>

/**
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */
pid_t getpid( void )
{
  return _Objects_Local_node;
}

#if defined(RTEMS_NEWLIB) && !defined(HAVE__GETPID_R)

#include <reent.h>

/**
 *  This is the Newlib dependent reentrant version of getpid().
 */
pid_t _getpid_r(
  struct _reent *ptr __attribute__((unused))
)
{
  return getpid();
}
#endif
