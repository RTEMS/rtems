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
#include <rtems/bspsmp.h>
#include <rtems/score/smp.h>
#include <rtems/score/thread.h>

#if defined(RTEMS_SMP)
#define SMP_DEBUG

#if defined(SMP_DEBUG)
  #include <rtems/bspIo.h>
#endif

void rtems_smp_run_first_task(int cpu)
{
  Thread_Control *heir;

  /*
   *  This CPU has an heir thread so we need to dispatch it.
   */
  heir = _Thread_Heir;

  /*
   *  This is definitely a hack until we have SMP scheduling.  Since there
   *  is only one executing and heir right now, we have to fake this out.
   */
  _Thread_Dispatch_set_disable_level(1);
  _Thread_Executing = heir;
  _CPU_Context_switch_to_first_task_smp( &heir->Registers );
}

void rtems_smp_secondary_cpu_initialize(void)
{
  int cpu;

  cpu = bsp_smp_processor_id();

  bsp_smp_secondary_cpu_initialize(cpu);

  #if defined(SMP_DEBUG)
    printk( "Made it to %d -- ", cpu );
  #endif

  /*
   *  Inform the primary CPU that this secondary CPU is initialized
   *  and ready to dispatch to the first thread it is supposed to
   *  execute when the primary CPU is ready.
   */
  _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_INITIALIZED;

  /*
   *  HACK: Should not have to enable interrupts in real system here.
   *        It should happen as part of switching to the first task.
   */
   
  _Per_CPU_Information[cpu].isr_nest_level = 1;
  _ISR_Set_level( 0 );
  while(1) ;
}

void rtems_smp_process_interrupt(void)
{
  int        cpu;
  uint32_t   message;
  ISR_Level  level;

  cpu = bsp_smp_processor_id();

  level = _SMP_lock_Spinlock_Obtain( &_Per_CPU_Information[cpu].lock );
    message = _Per_CPU_Information[cpu].message;
    _Per_CPU_Information[cpu].message &= ~message;
  _SMP_lock_Spinlock_Release( &_Per_CPU_Information[cpu].lock, level );

  #if defined(SMP_DEBUG)
    {
      void *sp = __builtin_frame_address(0);
      if ( !(message & RTEMS_BSP_SMP_SHUTDOWN) )
	printk( "ISR on CPU %d -- (0x%02x) (0x%p)\n", cpu, message, sp );
      printk( "Dispatch level %d\n", _Thread_Dispatch_disable_level );
    }
  #endif

  if ( message & RTEMS_BSP_SMP_FIRST_TASK ) {
    _Per_CPU_Information[cpu].isr_nest_level = 0;
    _Per_CPU_Information[cpu].message = 0;
    _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_INITIALIZED;
    rtems_smp_run_first_task(cpu);
    /* does not return */
  }

  if ( message & RTEMS_BSP_SMP_SHUTDOWN ) {
    ISR_Level level;
    _Thread_Dispatch_set_disable_level(0);
    _Per_CPU_Information[cpu].isr_nest_level = 0;
    _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_SHUTDOWN;
    _ISR_Disable( level );
    while(1)
      ;
    /* does not continue past here */
  }

  if ( message & RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY ) {
    printk( "switch needed\n" );
    _Per_CPU_Information[cpu].dispatch_necessary = true;
  }
}

void rtems_smp_send_message( 
  int       cpu,
  uint32_t  message
)
{
  ISR_Level level;

  level = _SMP_lock_Spinlock_Obtain( &_Per_CPU_Information[cpu].lock );
    _Per_CPU_Information[cpu].message |= message;
  _SMP_lock_Spinlock_Release( &_Per_CPU_Information[cpu].lock, level );
  bsp_smp_interrupt_cpu( cpu );
}

void rtems_smp_broadcast_message(
  uint32_t  message
)
{
  int        dest_cpu;
  int        cpu;
  ISR_Level  level;

  cpu = bsp_smp_processor_id();

  for ( dest_cpu=0 ; dest_cpu <  _SMP_Processor_count; dest_cpu++ ) {
    if ( cpu == dest_cpu )
      continue;
    level = _SMP_lock_Spinlock_Obtain( &_Per_CPU_Information[cpu].lock );
      _Per_CPU_Information[dest_cpu].message |= message;
    _SMP_lock_Spinlock_Release( &_Per_CPU_Information[cpu].lock, level );
  }
  bsp_smp_broadcast_interrupt();
}
#endif
