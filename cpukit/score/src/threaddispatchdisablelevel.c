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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/threaddispatch.h>
#include <rtems/score/assert.h>
#include <rtems/score/profiling.h>
#include <rtems/score/sysstate.h>

#define NO_OWNER_CPU NULL

typedef struct {
  SMP_lock_Control lock;
  Per_CPU_Control *owner_cpu;
  uint32_t nest_level;
} Giant_Control;

static Giant_Control _Giant = {
  .lock = SMP_LOCK_INITIALIZER("Giant"),
  .owner_cpu = NO_OWNER_CPU,
  .nest_level = 0
};

static void _Giant_Do_acquire( Per_CPU_Control *cpu_self )
{
  Giant_Control *giant = &_Giant;

  if ( giant->owner_cpu != cpu_self ) {
    _SMP_lock_Acquire( &giant->lock, &cpu_self->Giant_lock_context );
    giant->owner_cpu = cpu_self;
    giant->nest_level = 1;
  } else {
    ++giant->nest_level;
  }
}

static void _Giant_Do_release( Per_CPU_Control *cpu_self )
{
  Giant_Control *giant = &_Giant;

  --giant->nest_level;
  if ( giant->nest_level == 0 ) {
    giant->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &giant->lock, &cpu_self->Giant_lock_context );
  }
}

void _Giant_Drop( Per_CPU_Control *cpu_self )
{
  Giant_Control *giant = &_Giant;

  _Assert( _ISR_Get_level() != 0 );

  if ( giant->owner_cpu == cpu_self ) {
    giant->nest_level = 0;
    giant->owner_cpu = NO_OWNER_CPU;
    _SMP_lock_Release( &giant->lock, &cpu_self->Giant_lock_context );
  }
}

uint32_t _Thread_Dispatch_increment_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t disable_level;
  Per_CPU_Control *cpu_self;

  _ISR_Disable_without_giant( isr_level );

  /*
   * We must obtain the processor after interrupts are disabled to prevent
   * thread migration.
   */
  cpu_self = _Per_CPU_Get();

  _Giant_Do_acquire( cpu_self );

  disable_level = cpu_self->thread_dispatch_disable_level;
  _Profiling_Thread_dispatch_disable( cpu_self, disable_level );
  ++disable_level;
  cpu_self->thread_dispatch_disable_level = disable_level;

  _ISR_Enable_without_giant( isr_level );

  return disable_level;
}

uint32_t _Thread_Dispatch_decrement_disable_level( void )
{
  ISR_Level isr_level;
  uint32_t disable_level;
  Per_CPU_Control *cpu_self;

  _ISR_Disable_without_giant( isr_level );

  cpu_self = _Per_CPU_Get();
  disable_level = cpu_self->thread_dispatch_disable_level;
  --disable_level;
  cpu_self->thread_dispatch_disable_level = disable_level;

  _Giant_Do_release( cpu_self );
  _Assert( disable_level != 0 || _Giant.owner_cpu != cpu_self );

  _Profiling_Thread_dispatch_enable( cpu_self, disable_level );
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
bool _Debug_Is_owner_of_giant( void )
{
  Giant_Control *giant = &_Giant;

  return giant->owner_cpu == _Per_CPU_Get_snapshot()
    || !_System_state_Is_up( _System_state_Get() );
}
#endif
