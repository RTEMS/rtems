#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

#include <rtems/userenv.h>

/*
 * MACRO in userenv.h
uid_t _POSIX_types_Euid = 0;
*/

/*
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs,
 *        P1003.1b-1993, p. 84
 */

uid_t geteuid( void )
{
  return _POSIX_types_Euid;
}
