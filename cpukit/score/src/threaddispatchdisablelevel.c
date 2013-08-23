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

#include <rtems/score/threaddispatch.h>
#include <rtems/score/assert.h>

#define NO_OWNER_CPU 0xffffffffU

typedef struct {
  SMP_lock_Control lock;
  uint32_t owner_cpu;
  uint32_t nest_level;
} Giant_Control;

static Giant_Control _Giant = {
  .lock = SMP_LOCK_INITIALIZER,
  .owner_cpu = NO_OWNER_CPU,
  .nest_level = 0
};

static void _Giant_Do_acquire( uint32_t self_cpu_index )
{
  Giant_Control *giant = &_Giant;

  if ( giant->owner_cpu != self_cpu_index ) {
    _SMP_lock_Acquire( &giant->lock );
    giant->owner_cpu = self_cpu_index;
    giant->nest_level = 1;
  } else {
    ++giant->nest_level;
  }
}

static void _Giant_Do_release( void )
{
  Giant_Control *giant = &_Giant;

  --giant->nest_level;
  if ( giant->nest_level == 0 ) {
    giant->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &giant->lock );
  }
}

uint32_t _Thread_Dispatch_increment_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t self_cpu_index;
  uint32_t disable_level;
  Per_CPU_Control *self_cpu;

  _ISR_Disable( isr_level );

  /*
   * We must obtain the processor ID after interrupts are disabled to prevent
   * thread migration.
   */
  self_cpu_index = _SMP_Get_current_processor();

  _Giant_Do_acquire( self_cpu_index );

  self_cpu = _Per_CPU_Get_by_index( self_cpu_index );
  disable_level = self_cpu->thread_dispatch_disable_level;
  ++disable_level;
  self_cpu->thread_dispatch_disable_level = disable_level;

  _ISR_Enable( isr_level );

  return disable_level;
}

uint32_t _Thread_Dispatch_decrement_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t disable_level;
  Per_CPU_Control *self_cpu;

  _ISR_Disable( isr_level );

  self_cpu = _Per_CPU_Get();
  disable_level = self_cpu->thread_dispatch_disable_level;
  --disable_level;
  self_cpu->thread_dispatch_disable_level = disable_level;

  _Giant_Do_release();

  _ISR_Enable( isr_level );

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
  ISR_Level isr_level;
  uint32_t disable_level;

  _ISR_Disable( isr_level );
  disable_level = _Thread_Dispatch_disable_level;
  _ISR_Enable( isr_level );

  /*
   * If we need the dispatch level to go higher 
   * call increment method the desired number of times.
   */

  while ( value > disable_level ) {
    disable_level = _Thread_Dispatch_increment_disable_level();
  }

  /*
   * If we need the dispatch level to go lower
   * call increment method the desired number of times.
   */

  while ( value < disable_level ) {
    disable_level = _Thread_Dispatch_decrement_disable_level();
  }

  return value;
}

void _Giant_Acquire( void )
{
  ISR_Level isr_level;

  _ISR_Disable( isr_level );
  _Assert( _Thread_Dispatch_disable_level != 0 );
  _Giant_Do_acquire( _SMP_Get_current_processor() );
  _ISR_Enable( isr_level );
}

void _Giant_Release( void )
{
  ISR_Level isr_level;

  _ISR_Disable( isr_level );
  _Assert( _Thread_Dispatch_disable_level != 0 );
  _Giant_Do_release();
  _ISR_Enable( isr_level );
}
