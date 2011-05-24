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
#include <rtems/score/smp.h>

#if defined (RTEMS_DEBUG)
  #include <rtems/bspIo.h>
#endif

void _SMP_lock_spinlock_simple_Initialize(
  SMP_lock_spinlock_simple_Control *lock
)
{
  *lock = 0;
}

ISR_Level _SMP_lock_spinlock_simple_Obtain(
  SMP_lock_spinlock_simple_Control *lock
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

void _SMP_lock_spinlock_simple_Release(
  SMP_lock_spinlock_simple_Control *lock,
  ISR_Level                        level
)
{
   *lock = 0;
   _ISR_Enable( level );
}

void _SMP_lock_spinlock_nested_Initialize(
  SMP_lock_spinlock_nested_Control *lock
)
{
  lock->count = 0;
  lock->cpu_id = 0;
}

ISR_Level _SMP_lock_spinlock_nested_Obtain(
  SMP_lock_spinlock_nested_Control *lock
)
{
  ISR_Level  level = 0;
  uint32_t   value = 1;
  uint32_t   previous;
  int        cpu_id;

  /* Note: Disable provides an implicit memory barrier. */
  _ISR_Disable( level );

  cpu_id = bsp_smp_processor_id();

  /* Deal with nested calls from one cpu */
  if ( (lock->count > 0) && (cpu_id == lock->cpu_id) ) {
    lock->count++;
    return level;
  }

  do {
    SMP_CPU_SWAP( lock, value, previous );
  } while (previous == 1);

  lock->count++;
  lock->cpu_id = cpu_id;

  return level;
}

void _SMP_lock_spinlock_nested_Release(
  SMP_lock_spinlock_nested_Control *lock,
  ISR_Level                        level
)
{
#if defined(RTEMS_DEBUG)
  if ( lock->count == 0 )
    printk ("Releasing spinlock when count is already zero?!?!\n");
#endif
  lock->count--;

  _ISR_Enable( level );
}
