/*
 *  Thread Dispatch Disable Level Methods
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>

void _Thread_Dispatch_initialization( void )
{
  _Thread_Dispatch_disable_level = 0; 
  _SMP_lock_spinlock_nested_Initialize(&_Thread_Dispatch_disable_level_lock);
  _Thread_Dispatch_set_disable_level( 1 );
}

bool _Thread_Dispatch_in_critical_section(void)
{
  if (  _Thread_Dispatch_disable_level == 0 )
   return false;

  return true;
}

uint32_t _Thread_Dispatch_get_disable_level(void)
{
  return _Thread_Dispatch_disable_level;
}

uint32_t _Thread_Dispatch_increment_disable_level(void)
{
  ISR_Level  isr_level;
  uint32_t   level;

  /*
   * Note: _SMP_lock_spinlock_nested_Obtain returns
   *       with ISR's disabled and the isr_level that
   *       should be restored after a short period.
   *
   * Here we obtain the lock and increment the 
   * Thread dispatch disable level while under the
   * protection of the isr being off.  After this
   * point it is safe to re-enable ISRs and allow
   * the dispatch disable lock to provide protection.
   */

  isr_level = _SMP_lock_spinlock_nested_Obtain(
    &_Thread_Dispatch_disable_level_lock
  );
  
  _Thread_Dispatch_disable_level++;
  level = _Thread_Dispatch_disable_level;

  _ISR_Enable_on_this_core(isr_level);
  return level;
}

uint32_t _Thread_Dispatch_decrement_disable_level(void)
{
  ISR_Level  isr_level;
  uint32_t   level;

  /*  First we must disable ISRs in order to protect
   *  accesses to the dispatch disable level.
   */
  _ISR_Disable_on_this_core( isr_level );

  _Thread_Dispatch_disable_level--;
  level = _Thread_Dispatch_disable_level;


  /* 
   * Note: _SMP_lock_spinlock_nested_Obtain returns with
   *        ISR's disabled and _SMP_lock_spinlock_nested_Release
   *        is responsable for re-enabling interrupts.
   */
  _SMP_lock_spinlock_nested_Release( 
    &_Thread_Dispatch_disable_level_lock,
    isr_level
  ); 

  return level;
}


/*
 * Note this method is taking a heavy handed approach to 
 * setting the dispatch level. This may be optimized at a 
 * later timee, but it must be in such a way that the nesting
 * level is decremented by the same number as the dispatch level.
 * This approach is safest until we are sure the nested spinlock
 * is successfully working with smp isr source code.  
 */

uint32_t _Thread_Dispatch_set_disable_level(uint32_t value)
{
  /*
   * If we need the dispatch level to go higher 
   * call increment method the desired number of times.
   */

  while ( value > _Thread_Dispatch_disable_level ) {
    _Thread_Dispatch_increment_disable_level();
  }

  /*
   * If we need the dispatch level to go lower
   * call increment method the desired number of times.
   */

  while ( value < _Thread_Dispatch_disable_level ) {
    _Thread_Dispatch_decrement_disable_level();
  }

  return value;
}
