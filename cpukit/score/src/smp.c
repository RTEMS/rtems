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

#include <rtems/bspsmp.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/smp.h>
#include <rtems/score/sysstate.h>

#if defined(RTEMS_DEBUG)
  #include <rtems/bspIo.h>
#endif

void rtems_smp_secondary_cpu_initialize( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();

  #if defined(RTEMS_DEBUG)
    printk( "Made it to %d -- ", _Per_CPU_Get_index( self_cpu ) );
  #endif

  _Per_CPU_Change_state( self_cpu, PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING );

  _Per_CPU_Wait_for_state( self_cpu, PER_CPU_STATE_BEGIN_MULTITASKING );

  _Thread_Start_multitasking( NULL );
}

void rtems_smp_process_interrupt( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();


  if ( self_cpu->message != 0 ) {
    uint32_t  message;
    ISR_Level level;

    _Per_CPU_ISR_disable_and_acquire( self_cpu, level );
    message = self_cpu->message;
    self_cpu->message = 0;
    _Per_CPU_Release_and_ISR_enable( self_cpu, level );

    #if defined(RTEMS_DEBUG)
      {
        void *sp = __builtin_frame_address(0);
        if ( !(message & RTEMS_BSP_SMP_SHUTDOWN) ) {
          printk(
            "ISR on CPU %d -- (0x%02x) (0x%p)\n",
            _Per_CPU_Get_index( self_cpu ),
            message,
            sp
          );
          if ( message & RTEMS_BSP_SMP_SHUTDOWN )
            printk( "shutdown\n" );
        }
        printk( "Dispatch level %d\n", _Thread_Dispatch_get_disable_level() );
      }
    #endif

    if ( ( message & RTEMS_BSP_SMP_SHUTDOWN ) != 0 ) {
      _ISR_Disable_without_giant( level );

      _Thread_Dispatch_set_disable_level( 0 );

      _Per_CPU_Change_state( self_cpu, PER_CPU_STATE_SHUTDOWN );

      _CPU_Fatal_halt( _Per_CPU_Get_index( self_cpu ) );
      /* does not continue past here */
    }
  }
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

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    if ( cpu != self ) {
      Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index( cpu );
      ISR_Level level;

      _Per_CPU_ISR_disable_and_acquire( per_cpu, level );
      per_cpu->message |= message;
      _Per_CPU_Release_and_ISR_enable( per_cpu, level );
    }
  }

  bsp_smp_broadcast_interrupt();
}

void _SMP_Request_other_cores_to_perform_first_context_switch( void )
{
  uint32_t self = _SMP_Get_current_processor();
  uint32_t ncpus = _SMP_Get_processor_count();
  uint32_t cpu;

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index( cpu );

    if ( cpu != self ) {
      _Per_CPU_Change_state( per_cpu, PER_CPU_STATE_BEGIN_MULTITASKING );
    }
  }
}

void _SMP_Request_other_cores_to_shutdown( void )
{
  uint32_t self = _SMP_Get_current_processor();
  uint32_t ncpus = _SMP_Get_processor_count();
  uint32_t cpu;

  _SMP_Broadcast_message( RTEMS_BSP_SMP_SHUTDOWN );

  for ( cpu = 0 ; cpu < ncpus ; ++cpu ) {
    if ( cpu != self ) {
      _Per_CPU_Wait_for_state(
        _Per_CPU_Get_by_index( cpu ),
        PER_CPU_STATE_SHUTDOWN
      );
    }
  }
}
