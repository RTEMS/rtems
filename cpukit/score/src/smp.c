/**
 *  @file
 *
 *  @brief SMP Support
 *  @ingroup Score
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/bspsmp.h>
#include <rtems/score/smp.h>
#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>

#if defined(RTEMS_DEBUG)
  #include <rtems/bspIo.h>
#endif

void rtems_smp_secondary_cpu_initialize( void )
{
  int              self = bsp_smp_processor_id();
  Per_CPU_Control *per_cpu = &_Per_CPU_Information[ self ];
  Thread_Control  *heir;

  #if defined(RTEMS_DEBUG)
    printk( "Made it to %d -- ", self );
  #endif

  _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING );

  _Per_CPU_Wait_for_state( per_cpu, PER_CPU_STATE_BEGIN_MULTITASKING );

  _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_UP );

  /*
   *  The Scheduler will have selected the heir thread for each CPU core.
   *  Now we have been requested to perform the first context switch.  So
   *  force a switch to the designated heir and make it executing on
   *  THIS core.
   */
  heir = per_cpu->heir;
  heir->is_executing = true;
  per_cpu->executing->is_executing = false;
  per_cpu->executing = heir;
  per_cpu->dispatch_necessary = false;

  /*
   * Threads begin execution in the _Thread_Handler() function.   This function
   * will call _Thread_Enable_dispatch().
   */
  _Thread_Disable_dispatch();

  _CPU_Context_switch_to_first_task_smp( &heir->Registers );
}

void rtems_smp_process_interrupt( void )
{
  int              self = bsp_smp_processor_id();
  Per_CPU_Control *per_cpu = &_Per_CPU_Information[ self ];
  uint32_t         message;
  ISR_Level        level;


  _Per_CPU_Lock_acquire( per_cpu, level );
  message = per_cpu->message;
  per_cpu->message = 0;
  _Per_CPU_Lock_release( per_cpu, level );

  #if defined(RTEMS_DEBUG)
    {
      void *sp = __builtin_frame_address(0);
      if ( !(message & RTEMS_BSP_SMP_SHUTDOWN) ) {
        printk( "ISR on CPU %d -- (0x%02x) (0x%p)\n", self, message, sp );
	if ( message & RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY )
	  printk( "context switch necessary\n" );
	if ( message & RTEMS_BSP_SMP_SIGNAL_TO_SELF )
	  printk( "signal to self\n" );
	if ( message & RTEMS_BSP_SMP_SHUTDOWN )
	  printk( "shutdown\n" );
      }
 
      printk( "Dispatch level %d\n", _Thread_Dispatch_get_disable_level() );
    }
  #endif

  if ( message & RTEMS_BSP_SMP_SHUTDOWN ) {
    _ISR_Disable_on_this_core( level );

    while ( _Thread_Dispatch_decrement_disable_level() != 0 ) {
      /* Release completely */
    }

    _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_SHUTDOWN );

    _CPU_Fatal_halt( self );
    /* does not continue past here */
  }
}

void _SMP_Send_message( int cpu, uint32_t message )
{
  Per_CPU_Control *per_cpu = &_Per_CPU_Information[ cpu ];
  ISR_Level level;

  #if defined(RTEMS_DEBUG)
    if ( message & RTEMS_BSP_SMP_SIGNAL_TO_SELF )
      printk( "Send 0x%x to %d\n", message, cpu );
  #endif

  _Per_CPU_Lock_acquire( per_cpu, level );
  per_cpu->message |= message;
  _Per_CPU_Lock_release( per_cpu, level );

  bsp_smp_interrupt_cpu( cpu );
}

void _SMP_Broadcast_message( uint32_t message )
{
  int self = bsp_smp_processor_id();
  int ncpus = _SMP_Get_processor_count();
  int cpu;

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    if ( cpu != self ) {
      Per_CPU_Control *per_cpu = &_Per_CPU_Information[ cpu ];
      ISR_Level level;

      _Per_CPU_Lock_acquire( per_cpu, level );
      per_cpu->message |= message;
      _Per_CPU_Lock_release( per_cpu, level );
    }
  }

  bsp_smp_broadcast_interrupt();
}

void _SMP_Request_other_cores_to_perform_first_context_switch( void )
{
  int self = bsp_smp_processor_id();
  int ncpus = _SMP_Get_processor_count();
  int cpu;

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    Per_CPU_Control *per_cpu = &_Per_CPU_Information[ cpu ];

    if ( cpu != self ) {
      _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_BEGIN_MULTITASKING );
    } else {

      _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_UP );
    }
  }
}

void _SMP_Request_other_cores_to_dispatch( void )
{
  if ( _System_state_Is_up( _System_state_Get() ) ) {
    int self = bsp_smp_processor_id();
    int ncpus = _SMP_Get_processor_count();
    int cpu;

    for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
      const Per_CPU_Control *per_cpu = &_Per_CPU_Information[ cpu ];

      if (
        cpu != self
          && per_cpu->state == PER_CPU_STATE_UP
          && per_cpu->dispatch_necessary
      ) {
        _SMP_Send_message( cpu, RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY );
      }
    }
  }
}

void _SMP_Request_other_cores_to_shutdown( void )
{
  int self = bsp_smp_processor_id();
  int ncpus = _SMP_Get_processor_count();
  int cpu;

  _SMP_Broadcast_message( RTEMS_BSP_SMP_SHUTDOWN );

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    if ( cpu != self ) {
      _Per_CPU_Wait_for_state(
        &_Per_CPU_Information[ cpu ],
        PER_CPU_STATE_SHUTDOWN
      );
    }
  }
}
