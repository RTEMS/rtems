/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

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
