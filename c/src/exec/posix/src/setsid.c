/*
 *  $Id$
 */

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/seterr.h>

/*PAGE
 *
 *  4.3.2 Create Session and Set Process Group ID, P1003.1b-1993, p. 88
 */

pid_t setsid( void )
{
  set_errno_and_return_minus_one( EPERM );
}


