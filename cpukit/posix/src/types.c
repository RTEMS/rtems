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

pid_t _POSIX_types_Ppid = 0;
uid_t _POSIX_types_Uid = 0;
uid_t _POSIX_types_Euid = 0;
gid_t _POSIX_types_Gid = 0;
gid_t _POSIX_types_Egid = 0;

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getpid( void )
{
  return _Objects_Local_node;
}

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getppid( void )
{
  return _POSIX_types_Ppid;
}

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
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

uid_t geteuid( void )
{
  return _POSIX_types_Euid;
}

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
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

gid_t getegid( void )
{
  return _POSIX_types_Egid;
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

/*PAGE
 *
 *  4.2.3 Get Supplementary IDs, P1003.1b-1993, p. 86
 */

int getgroups(
  int    gidsetsize,
  gid_t  grouplist[]
)
{
  return 0;  /* no supplemental group ids */
}

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

static char _POSIX_types_Getlogin_buffer[ LOGIN_NAME_MAX ];

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
  if ( namesize < LOGIN_NAME_MAX )
    return ERANGE;

  strcpy( name, "posixapp" );
  return 0;
}

/*PAGE
 *
 *  4.3.1 Get Process Group IDs, P1003.1b-1993, p. 89
 */

pid_t getpgrp( void )
{
  /*
   *  This always succeeds and returns the process group id.  For rtems,
   *  this will always be the local node;
   */

  return _Objects_Local_node;
}

/*PAGE
 *
 *  4.3.2 Create Session and Set Process Group ID, P1003.1b-1993, p. 88
 */

pid_t setsid( void )
{
  set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  4.3.3 Set Process Group ID for Job Control, P1003.1b-1993, p. 89
 */

int setpgid(
  pid_t  pid,
  pid_t  pgid
)
{
  set_errno_and_return_minus_one( ENOSYS );
}

/*
 * TEMPORARY
 */

#include <assert.h>

int POSIX_MP_NOT_IMPLEMENTED()
{
  assert( 0 );
  return 0;
}

int POSIX_BOTTOM_REACHED()
{
  assert( 0 );
  return 0;
}

int POSIX_NOT_IMPLEMENTED()
{
  assert( 0 );
  return 0;
}

/*
 * END OF TEMPORARY
 */

