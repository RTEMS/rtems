/**
 * @file
 *
 * @brief Thread Dispatch Disable Functions
 *
 * @ingroup ScoreThread
 */

/*
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
#include <rtems/score/threaddispatch.h>

#define NO_OWNER_CPU 0xffffffffU

void _Thread_Dispatch_initialization( void )
{
  Thread_Dispatch_disable_level_lock_control *level_lock =
    &_Thread_Dispatch_disable_level_lock;

  _Thread_Dispatch_disable_level = 0;
  _SMP_lock_Initialize( &level_lock->lock );
  level_lock->owner_cpu = NO_OWNER_CPU;
  _Thread_Dispatch_set_disable_level( 1 );
}

uint32_t _Thread_Dispatch_get_disable_level(void)
{
  return _Thread_Dispatch_disable_level;
}

uint32_t _Thread_Dispatch_increment_disable_level( void )
{
  Thread_Dispatch_disable_level_lock_control *level_lock =
    &_Thread_Dispatch_disable_level_lock;
  ISR_Level isr_level;
  uint32_t self_cpu;
  uint32_t disable_level;

  _ISR_Disable_on_this_core( isr_level );

  /*
   * We must obtain the processor ID after interrupts are disabled since a
   * non-optimizing compiler may store the value on the stack and read it back.
   */
  self_cpu = _SMP_Get_current_processor();

  if ( level_lock->owner_cpu != self_cpu ) {
    _SMP_lock_Acquire( &level_lock->lock );
    level_lock->owner_cpu = self_cpu;
    level_lock->nest_level = 1;
  } else {
    ++level_lock->nest_level;
  }

  disable_level = _Thread_Dispatch_disable_level;
  ++disable_level;
  _Thread_Dispatch_disable_level = disable_level;

  _ISR_Enable_on_this_core( isr_level );

  return disable_level;
}

uint32_t _Thread_Dispatch_decrement_disable_level( void )
{
  Thread_Dispatch_disable_level_lock_control *level_lock =
    &_Thread_Dispatch_disable_level_lock;
  ISR_Level isr_level;
  uint32_t disable_level;

  _ISR_Disable_on_this_core( isr_level );

  disable_level = _Thread_Dispatch_disable_level;
  --disable_level;
  _Thread_Dispatch_disable_level = disable_level;

  --level_lock->nest_level;
  if ( level_lock->nest_level == 0 ) {
    level_lock->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &level_lock->lock );
  }

  _ISR_Enable_on_this_core( isr_level );

  return disable_level;
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
