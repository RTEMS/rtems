#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/seterr.h>

/*
 *  4.3.2 Create Session and Set Process Group ID, P1003.1b-1993, p. 88
 */

pid_t setsid( void )
{
  rtems_set_errno_and_return_minus_one( EPERM );
}
