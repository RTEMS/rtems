/*  utsname.c 
 *
 *  $Id$
 */

#include <stdio.h>
#include <string.h>

#include <sys/utsname.h>

#include <rtems/system.h>
#include <rtems/score/system.h>
#include <rtems/score/object.h>

/*PAGE
 *
 *  4.4.1 Get System Name, P1003.1b-1993, p. 90
 */

int uname(
  struct utsname *name
)
{
  /*  XXX: Here is what Solaris returns...
          sysname = SunOS
          nodename = node_name
          release = 5.3
          version = Generic_101318-12
          machine = sun4m
  */

  strcpy( name->sysname, "RTEMS" );

  sprintf( name->nodename, "Node %d\n", _Objects_Local_node );

  /* XXX release string is in BAD format for this routine!!! */ 
  strcpy( name->release, "3.2.0" );
 
  /* XXX does this have any meaning for RTEMS */
   
  strcpy( name->release, "" );

  sprintf( name->machine, "%s/%s", CPU_NAME, CPU_MODEL_NAME );

  return 0;
}

#ifdef NOT_IMPLEMENTED_YET

/*PAGE
 *
 *  4.5.2 Get Process Times, P1003.1b-1993, p. 92
 */

clock_t times(
  struct tms   *buffer
)
{
  return POSIX_NOT_IMPLEMENTED();
}

#endif
