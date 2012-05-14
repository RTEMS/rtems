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

/*
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */
char *getlogin( void )
{
  (void) getlogin_r( _POSIX_types_Getlogin_buffer, LOGIN_NAME_MAX );
  return _POSIX_types_Getlogin_buffer;
}

/*
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
  char          *pname;

  if ( !name ) 
    return EFAULT;

  if ( namesize < LOGIN_NAME_MAX )
    return ERANGE;

  /* Set the pointer to a default name */
  pname = "";

  pw = getpwuid(getuid());
  if ( pw )
   pname = pw->pw_name;

  strncpy( name, pname, LOGIN_NAME_MAX );
  return 0;
}
