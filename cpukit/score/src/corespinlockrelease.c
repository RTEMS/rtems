/*
 *  SuperCore Spinlock Handler -- Release a Spinlock
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

#include <rtems/system.h>
#include <rtems/score/corespinlock.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _CORE_spinlock_Release
 *
 *  This function releases the spinlock.
 *
 *  Input parameters:
 *    the_spinlock    - the spinlock control block to initialize
 *
 *  Output parameters:  NONE
 */

CORE_spinlock_Status _CORE_spinlock_Release(
  CORE_spinlock_Control  *the_spinlock
)
{
  ISR_Level level;

  _ISR_Disable( level );
    if ( the_spinlock->lock == CORE_SPINLOCK_UNLOCKED ) {
      _ISR_Enable( level );
      return CORE_SPINLOCK_NOT_LOCKED;
    } 
    
    the_spinlock->users -= 1;
    the_spinlock->lock   = CORE_SPINLOCK_UNLOCKED;
    the_spinlock->holder = 0;
  _ISR_Enable( level );
  return CORE_SPINLOCK_SUCCESSFUL;
}
