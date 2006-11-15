/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>

/*PAGE
 *
 *  RWLock Attributes Initialization
 */

int pthread_rwlockattr_init(
  pthread_rwlockattr_t *attr
)
{
  if ( !attr )
    return EINVAL;

  attr->is_initialized = TRUE;
  attr->process_shared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}
