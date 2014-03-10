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
#include <rtems/score/sysstate.h>

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

static void _Giant_Do_acquire( Per_CPU_Control *self_cpu )
{
  Giant_Control *giant = &_Giant;
  uint32_t self_cpu_index = _Per_CPU_Get_index( self_cpu );

  if ( giant->owner_cpu != self_cpu_index ) {
    _SMP_lock_Acquire( &giant->lock, &self_cpu->Giant_lock_context );
    giant->owner_cpu = self_cpu_index;
    giant->nest_level = 1;
  } else {
    ++giant->nest_level;
  }
}

static void _Giant_Do_release( Per_CPU_Control *self_cpu )
{
  Giant_Control *giant = &_Giant;

  --giant->nest_level;
  if ( giant->nest_level == 0 ) {
    giant->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &giant->lock, &self_cpu->Giant_lock_context );
  }
}

void _Giant_Drop( Per_CPU_Control *self_cpu )
{
  Giant_Control *giant = &_Giant;
  uint32_t self_cpu_index = _Per_CPU_Get_index( self_cpu );

  _Assert( _ISR_Get_level() != 0 );

  if ( giant->owner_cpu == self_cpu_index ) {
    giant->nest_level = 0;
    giant->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &giant->lock, &self_cpu->Giant_lock_context );
  }
}

uint32_t _Thread_Dispatch_increment_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t disable_level;
  Per_CPU_Control *self_cpu;

  _ISR_Disable_without_giant( isr_level );

  /*
   * We must obtain the processor after interrupts are disabled to prevent
   * thread migration.
   */
  self_cpu = _Per_CPU_Get();

  _Giant_Do_acquire( self_cpu );

  disable_level = self_cpu->thread_dispatch_disable_level;
  ++disable_level;
  self_cpu->thread_dispatch_disable_level = disable_level;

  _ISR_Enable_without_giant( isr_level );

  return disable_level;
}

uint32_t _Thread_Dispatch_decrement_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t disable_level;
  Per_CPU_Control *self_cpu;

  _ISR_Disable_without_giant( isr_level );

  self_cpu = _Per_CPU_Get();
  disable_level = self_cpu->thread_dispatch_disable_level;
  --disable_level;
  self_cpu->thread_dispatch_disable_level = disable_level;

  _Giant_Do_release( self_cpu );
  _Assert( disable_level != 0 || _Giant.owner_cpu == NO_OWNER_CPU );

  _ISR_Enable_without_giant( isr_level );

  return disable_level;
}

void _Giant_Acquire( void )
{
  ISR_Level isr_level;

  _ISR_Disable_without_giant( isr_level );
  _Assert( _Thread_Dispatch_disable_level != 0 );
  _Giant_Do_acquire( _Per_CPU_Get() );
  _ISR_Enable_without_giant( isr_level );
}

void _Giant_Release( void )
{
  ISR_Level isr_level;

  _ISR_Disable_without_giant( isr_level );
  _Assert( _Thread_Dispatch_disable_level != 0 );
  _Giant_Do_release( _Per_CPU_Get() );
  _ISR_Enable_without_giant( isr_level );
}

#if defined( RTEMS_DEBUG )
void _Assert_Owner_of_giant( void )
{
  Giant_Control *giant = &_Giant;

  _Assert(
    giant->owner_cpu == _SMP_Get_current_processor()
      || !_System_state_Is_up( _System_state_Get() )
  );
}
#endif
