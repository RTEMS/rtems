/*
 *  POSIX Spinlock Manager -- Initialize a Spinlock Instance
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
#include <rtems/posix/spinlock.h>

/*
 *  pthread_spinlock_init
 *
 *  This directive creates a spinlock.  A spinlock id is returned.
 *
 *  Input parameters:
 *    spinlock         - pointer to spinlock id
 *    pshared          - is this spinlock shared between processes
 *
 *  Output parameters:
 *    spinlock     - spinlock id
 *    0           - if successful
 *    error code  - if unsuccessful
 */

int pthread_spin_init(
  pthread_spinlock_t  *spinlock,
  int                  pshared
)
{
  POSIX_Spinlock_Control   *the_spinlock;
  CORE_spinlock_Attributes  attributes;


  if ( !spinlock )
    return EINVAL;

  switch ( pshared ) {
    case PTHREAD_PROCESS_PRIVATE:    /* only supported values */
      break;
    case PTHREAD_PROCESS_SHARED:
    default:
      return EINVAL;
  }

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_spinlock = _POSIX_Spinlock_Allocate();

  if ( !the_spinlock ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  _CORE_spinlock_Initialize_attributes( &attributes );

  _CORE_spinlock_Initialize( &the_spinlock->Spinlock, &attributes );

  _Objects_Open_u32( &_POSIX_Spinlock_Information, &the_spinlock->Object, 0 );

  *spinlock = the_spinlock->Object.id;

  _Thread_Enable_dispatch();
  return 0;
}
