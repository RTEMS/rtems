/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

/*
 *  _POSIX_Key_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Key_Manager_initialization(
  unsigned32 maximum_keys
)
{
  _Objects_Initialize_information(
    &_POSIX_Keys_Information,
    OBJECTS_POSIX_KEYS,
    FALSE,
    maximum_keys,
    sizeof( POSIX_Keys_Control ),
    FALSE,
    0,
    FALSE
  );
}
