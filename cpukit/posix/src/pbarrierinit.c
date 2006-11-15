/*
 *  POSIX Barrier Manager -- Initialize a Barrier Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/barrier.h>

/*
 *  pthread_barrier_init
 *
 *  This directive creates a barrier.  A barrier id is returned.
 *
 *  Input parameters:
 *    barrier          - pointer to barrier id
 *    attr             - barrier attributes
 *    count            - number of threads before automatic release
 *
 *  Output parameters:
 *    barrier     - barrier id
 *    0           - if successful
 *    error code  - if unsuccessful
 */

int pthread_barrier_init(
  pthread_barrier_t           *barrier,
  const pthread_barrierattr_t *attr,
  unsigned int                 count
)
{
  POSIX_Barrier_Control   *the_barrier;
  CORE_barrier_Attributes  the_attributes;
  

  if ( !barrier )
    return EINVAL;

  if ( count == 0 )
    return EINVAL;

  if ( !attr )
    return EINVAL;

  if ( !attr->is_initialized )
    return EINVAL;

  switch ( attr->process_shared ) {
    case PTHREAD_PROCESS_PRIVATE:    /* only supported values */
      break;
    case PTHREAD_PROCESS_SHARED:
    default:
      return EINVAL;
  }

  the_attributes.discipline    = CORE_BARRIER_AUTOMATIC_RELEASE;
  the_attributes.maximum_count = count;

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_barrier = _POSIX_Barrier_Allocate();

  if ( !the_barrier ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  _CORE_barrier_Initialize( &the_barrier->Barrier, &the_attributes );

  _Objects_Open(
    &_POSIX_Barrier_Information,
    &the_barrier->Object,
    0
  );

  *barrier = the_barrier->Object.id;

  _Thread_Enable_dispatch();
  return 0;
}
