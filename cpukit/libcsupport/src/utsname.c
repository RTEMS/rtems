#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <inttypes.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

/*
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

  if ( !name )
    rtems_set_errno_and_return_minus_one( EFAULT );

  strncpy( name->sysname, "RTEMS", sizeof(name->sysname) );

  snprintf( name->nodename, sizeof(name->nodename), "Node %" PRId16, _Objects_Local_node );

  strncpy( name->release, RTEMS_VERSION, sizeof(name->release) );

  strncpy( name->version, "", sizeof(name->version) );

  snprintf( name->machine, sizeof(name->machine), "%s/%s", CPU_NAME, CPU_MODEL_NAME );

  return 0;
}
