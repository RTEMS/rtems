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
gid_t _POSIX_types_Gid = 0;
*/

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

gid_t getgid( void )
{
  return _POSIX_types_Gid;
}

/*PAGE
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */

int setgid(
  gid_t  gid
)
{
  _POSIX_types_Gid = gid;
  return 0;
}
