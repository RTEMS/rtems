/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>
#include <rtems/userenv.h>

#include <unistd.h>
#include <pwd.h>

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

/*
 * MACRO in userenv.h
 * 
static char _POSIX_types_Getlogin_buffer[ LOGIN_NAME_MAX ];
*/

char *getlogin( void )
{
  (void) getlogin_r( _POSIX_types_Getlogin_buffer, LOGIN_NAME_MAX );
  return _POSIX_types_Getlogin_buffer;
}

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

int getlogin_r(
  char   *name,
  size_t  namesize
)
{
  struct passwd *pw;	
  if ( namesize < LOGIN_NAME_MAX )
    return ERANGE;

  pw=getpwuid(getuid());
  if (!pw) {
   strcpy(name,"");
  } else {
   strncpy(name,pw->pw_name,LOGIN_NAME_MAX);
  };
  return 0;
}
