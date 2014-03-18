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

#include <rtems/score/smpimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

void _SMP_Handler_initialize( void )
{
  uint32_t max_cpus = rtems_configuration_get_maximum_processors();
  uint32_t cpu;

  for ( cpu = 0 ; cpu < max_cpus; ++cpu ) {
    Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index( cpu );

    _SMP_ticket_lock_Initialize( &per_cpu->Lock, "per-CPU" );
  }

  /*
   * Discover and initialize the secondary cores in an SMP system.
   */
  max_cpus = _CPU_SMP_Initialize( max_cpus );

  _SMP_Processor_count = max_cpus;
}

void _SMP_Request_start_multitasking( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();
  uint32_t ncpus = _SMP_Get_processor_count();
  uint32_t cpu;

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index( cpu );

    _Per_CPU_State_change( per_cpu, PER_CPU_STATE_REQUEST_START_MULTITASKING );
  }
}

void _SMP_Start_multitasking_on_secondary_processor( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  _Thread_Start_multitasking();
}

void _SMP_Request_shutdown( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_SHUTDOWN );

  /*
   * We have to drop the Giant lock here in order to give other processors the
   * opportunity to receive the inter-processor interrupts issued previously.
   * In case the executing thread still holds SMP locks, then other processors
   * already waiting for this SMP lock will spin forever.
   */
  _Giant_Drop( self_cpu );
}

void _SMP_Send_message( uint32_t cpu, uint32_t message )
{
  Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index( cpu );
  ISR_Level level;

  _Per_CPU_ISR_disable_and_acquire( per_cpu, level );
  per_cpu->message |= message;
  _Per_CPU_Release_and_ISR_enable( per_cpu, level );

  _CPU_SMP_Send_interrupt( cpu );
}

void _SMP_Broadcast_message( uint32_t message )
{
  uint32_t self = _SMP_Get_current_processor();
  uint32_t ncpus = _SMP_Get_processor_count();
  uint32_t cpu;

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    if ( cpu != self ) {
      _SMP_Send_message( cpu, message );
    }
  }
}
