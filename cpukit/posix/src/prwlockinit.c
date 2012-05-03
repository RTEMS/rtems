/*
 *  POSIX RWLock Manager -- Destroy a RWLock Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/rwlock.h>

/*
 *  pthread_rwlock_init
 *
 *  This directive creates a rwlock.  A rwlock id is returned.
 *
 *  Input parameters:
 *    rwlock          - pointer to rwlock id
 *    attr            - rwlock attributes
 *
 *  Output parameters:
 *    rwlock     - rwlock id
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_rwlock_init(
  pthread_rwlock_t           *rwlock,
  const pthread_rwlockattr_t *attr
)
{
  POSIX_RWLock_Control        *the_rwlock;
  CORE_RWLock_Attributes       the_attributes;
  pthread_rwlockattr_t         default_attr;
  const pthread_rwlockattr_t  *the_attr;

  /*
   *  Error check parameters
   */
  if ( !rwlock )
    return EINVAL;

  /*
   * If the user passed in NULL, use the default attributes
   */
  if ( attr ) {
    the_attr = attr;
  } else {
    (void) pthread_rwlockattr_init( &default_attr );
    the_attr = &default_attr;
  }

  /*
   * Now start error checking the attributes that we are going to use
   */
  if ( !the_attr->is_initialized )
    return EINVAL;

  switch ( the_attr->process_shared ) {
    case PTHREAD_PROCESS_PRIVATE:    /* only supported values */
      break;
    case PTHREAD_PROCESS_SHARED:
    default:
      return EINVAL;
  }

  /*
   * Convert from POSIX attributes to Core RWLock attributes
   * 
   * NOTE: Currently there are no core rwlock attributes
   */
  _CORE_RWLock_Initialize_attributes( &the_attributes );

  /*
   * Enter dispatching critical section to allocate and initialize RWLock
   */
  _Thread_Disable_dispatch();             /* prevents deletion */

  the_rwlock = _POSIX_RWLock_Allocate();

  if ( !the_rwlock ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  _CORE_RWLock_Initialize( &the_rwlock->RWLock, &the_attributes );

  _Objects_Open_u32(
    &_POSIX_RWLock_Information,
    &the_rwlock->Object,
    0
  );

  *rwlock = the_rwlock->Object.id;

  _Thread_Enable_dispatch();
  return 0;
}
