/* types.c
 *
 *  $Id$
 */

#include <sys/types.h>

#include <rtems/system.h>

/*
 * TEMPORARY
 */

#include <assert.h>

int POSIX_MP_NOT_IMPLEMENTED()
{
  assert( 0 );
}

int POSIX_BOTTOM_REACHED()
{
  assert( 0 );
}

int POSIX_NOT_IMPLEMENTED()
{
  assert( 0 );
}

/*
 * END OF TEMPORARY
 */

#ifdef NOT_IMPLEMENTED_YET

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getpid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getppid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

uid_t getuid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

uid_t geteuid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

gid_t getgid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

gid_t getegid( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */

int setuid(
  uid_t  uid
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */

int setgid(
  gid_t  gid
)
{
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

char *getlogin( void )
{
  return (char *)POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

char *getlogin_r( void )
{
  return (char *)POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.3.1 Get Process Group IDs, P1003.1b-1993, p. 89
 */

pid_t getpgrp( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  4.3.2 Create Session and Set Process Group ID, P1003.1b-1993, p. 88
 */

pid_t setsid( void )
{
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

#endif
