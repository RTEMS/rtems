/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/score/smplock.h>

void _SMP_lock_Spinlock_Initialize(
  SMP_lock_Control *lock
)
{
  *lock = 0;
}

ISR_Level _SMP_lock_Spinlock_Obtain(
  SMP_lock_Control *lock
)
{
  ISR_Level  level;
  uint32_t   value = 1;
  uint32_t   previous;

  /* Note: Disable provides an implicit memory barrier. */
  _ISR_Disable( level );
    do {
      SMP_CPU_SWAP( lock, value, previous );
    } while (previous == 1);
  return level;
}

void _SMP_lock_Spinlock_Release(
  SMP_lock_Control *lock,
  ISR_Level        level
)
{
  *lock = 0;
  _ISR_Enable( level );
}
