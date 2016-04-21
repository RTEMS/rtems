/**
 * @file
 *
 * @brief Allocate resources to use the read-write lock and Initialize it
 * @ingroup POSIXAPI
 */

/*
 *  POSIX RWLock Manager -- Destroy a RWLock Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/rwlockimpl.h>
#include <rtems/score/apimutex.h>

static bool _POSIX_RWLock_Check_id_and_auto_init(
  pthread_mutex_t *rwlock
)
{
  if ( rwlock == NULL ) {
    return false;
  }

  if ( *rwlock == PTHREAD_RWLOCK_INITIALIZER ) {
    int eno;

    _Once_Lock();

    if ( *rwlock == PTHREAD_RWLOCK_INITIALIZER ) {
      eno = pthread_rwlock_init( rwlock, NULL );
    } else {
      eno = 0;
    }

    _Once_Unlock();

    if ( eno != 0 ) {
      return false;
    }
  }

  return true;
}

POSIX_RWLock_Control *_POSIX_RWLock_Get(
  pthread_rwlock_t *rwlock,
  ISR_lock_Context *lock_context
)
{
  if ( !_POSIX_RWLock_Check_id_and_auto_init( rwlock ) ) {
    return NULL;
  }

  return (POSIX_RWLock_Control *) _Objects_Get_local(
    *rwlock,
    &_POSIX_RWLock_Information,
    lock_context
  );
}

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

  the_rwlock = _POSIX_RWLock_Allocate();

  if ( !the_rwlock ) {
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

  _CORE_RWLock_Initialize( &the_rwlock->RWLock );

  _Objects_Open_u32(
    &_POSIX_RWLock_Information,
    &the_rwlock->Object,
    0
  );

  *rwlock = the_rwlock->Object.id;

  _Objects_Allocator_unlock();
  return 0;
}
