/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

#include <rtems/userenv.h>

/* 
 * MACRO in userenv.h 
 *
uid_t _POSIX_types_Uid = 0;
*/

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

uid_t getuid( void )
{
  return _POSIX_types_Uid;
}

/*PAGE
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */

int setuid(
  uid_t  uid
)
{
  _POSIX_types_Uid = uid;
  return 0;
}
