/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

/*PAGE
 *
 *  Barrier Attributes Get Process Shared
 */

int pthread_barrierattr_getpshared(
  const pthread_barrierattr_t *attr,
  int                         *pshared
)
{
  if ( !attr )
    return EINVAL;

  if ( !attr->is_initialized )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}
