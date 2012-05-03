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

#if defined(RTEMS_DEBUG)
  #include <rtems/bspIo.h>
#endif

/*
 *  Process request to switch to the first task on a secondary core.
 */
void rtems_smp_run_first_task(int cpu)
{
  Thread_Control *heir;
  ISR_Level       level;

  _ISR_Disable_on_this_core( level );

  /*
   *  The Scheduler will have selected the heir thread for each CPU core.
   *  Now we have been requested to perform the first context switch.  So
   *  force a switch to the designated heir and make it executing on 
   *  THIS core.
   */
  heir              = _Thread_Heir;
  _Thread_Executing = heir;

  _CPU_Context_switch_to_first_task_smp( &heir->Registers );
}

/*
 *  Process request to initialize this secondary core.
 */
void rtems_smp_secondary_cpu_initialize(void)
{
  int       cpu;
  ISR_Level level;

  cpu = bsp_smp_processor_id();

  _ISR_Disable_on_this_core( level );
  bsp_smp_secondary_cpu_initialize(cpu);

  /*
   *  Inform the primary CPU that this secondary CPU is initialized
   *  and ready to dispatch to the first thread it is supposed to
   *  execute when the primary CPU is ready.
   */
  _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_INITIALIZED;

  #if defined(RTEMS_DEBUG)
    printk( "Made it to %d -- ", cpu );
  #endif

  /*
   *  With this secondary core out of reset, we can wait for the
   *  request to switch to the first task.
   */
  while(1) {
    uint32_t   message;

    bsp_smp_wait_for(
      (volatile unsigned int *)&_Per_CPU_Information[cpu].message,
      RTEMS_BSP_SMP_FIRST_TASK,
      10000
    );

    level = _SMP_lock_spinlock_simple_Obtain( &_Per_CPU_Information[cpu].lock );
      message = _Per_CPU_Information[cpu].message;
      if ( message & RTEMS_BSP_SMP_FIRST_TASK ) {
	_SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );
        _ISR_Set_level( 0 );
      }
     
    _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );
  }
}

/*
 *  Process an interrupt processor interrupt which indicates a request
 *  from another core.
 */
void rtems_smp_process_interrupt(void)
{
  int        cpu;
  uint32_t   message;
  ISR_Level  level;

  cpu = bsp_smp_processor_id();

  level = _SMP_lock_spinlock_simple_Obtain( &_Per_CPU_Information[cpu].lock );
  message = _Per_CPU_Information[cpu].message;

  #if defined(RTEMS_DEBUG)
    {
      void *sp = __builtin_frame_address(0);
      if ( !(message & RTEMS_BSP_SMP_SHUTDOWN) ) {
        printk( "ISR on CPU %d -- (0x%02x) (0x%p)\n", cpu, message, sp );
	if ( message & RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY )
	  printk( "context switch necessary\n" );
	if ( message & RTEMS_BSP_SMP_SIGNAL_TO_SELF )
	  printk( "signal to self\n" );
	if ( message & RTEMS_BSP_SMP_SHUTDOWN )
	  printk( "shutdown\n" );
	if ( message & RTEMS_BSP_SMP_FIRST_TASK )
	  printk( "switch to first task\n" );
      }
 
      printk( "Dispatch level %d\n", _Thread_Dispatch_get_disable_level() );
    }
  #endif

  if ( message & RTEMS_BSP_SMP_FIRST_TASK ) {
    _Per_CPU_Information[cpu].isr_nest_level = 0;
    _Per_CPU_Information[cpu].message &= ~message;
    _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_UP;

    _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );

    rtems_smp_run_first_task(cpu);
    /* does not return */
  }

  if ( message & RTEMS_BSP_SMP_SHUTDOWN ) {
    _Per_CPU_Information[cpu].message &= ~message;

    _Per_CPU_Information[cpu].isr_nest_level = 0;
    _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_SHUTDOWN;
    _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );

    _Thread_Enable_dispatch();       /* undo ISR code */
    _ISR_Disable_on_this_core( level );
    while(1)
      ;
    /* does not continue past here */
  }

  if ( message & RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY ) {
    #if defined(RTEMS_DEBUG)
      printk( "switch needed\n" );
    #endif
    _Per_CPU_Information[cpu].message &= ~message;
    _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );
  }
}

/*
 *  Send an interrupt processor request to another cpu.
 */
void _SMP_Send_message(
  int       cpu,
  uint32_t  message
)
{
  ISR_Level level;

  #if defined(RTEMS_DEBUG)
    if ( message & RTEMS_BSP_SMP_SIGNAL_TO_SELF )
      printk( "Send 0x%x to %d\n", message, cpu );
  #endif

  level = _SMP_lock_spinlock_simple_Obtain( &_Per_CPU_Information[cpu].lock );
    _Per_CPU_Information[cpu].message |= message;
  _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );
  bsp_smp_interrupt_cpu( cpu );
}

/*
 *  Send interrupt processor request to all other nodes
 */
void _SMP_Broadcast_message(
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
    level = _SMP_lock_spinlock_simple_Obtain( &_Per_CPU_Information[cpu].lock );
      _Per_CPU_Information[dest_cpu].message |= message;
    _SMP_lock_spinlock_simple_Release( &_Per_CPU_Information[cpu].lock, level );
  }
  bsp_smp_broadcast_interrupt();
}

/*
 *  Send interrupt processor requests to perform first context switch 
 */
void _SMP_Request_other_cores_to_perform_first_context_switch(void)
{
  int    cpu;

  _Per_CPU_Information[cpu].state = RTEMS_BSP_SMP_CPU_UP; 
  for (cpu=1 ; cpu < _SMP_Processor_count ; cpu++ ) {
    _SMP_Send_message( cpu, RTEMS_BSP_SMP_FIRST_TASK );
  }
}

/*
 *  Send message to other cores requesting them to perform
 *  a thread dispatch operation.
 */
void _SMP_Request_other_cores_to_dispatch(void)
{
  int i;
  int cpu;

  cpu = bsp_smp_processor_id();

  if ( !_System_state_Is_up (_System_state_Current) )
    return;
  for (i=1 ; i < _SMP_Processor_count ; i++ ) {
    if ( cpu == i )
      continue;
    if ( _Per_CPU_Information[i].state != RTEMS_BSP_SMP_CPU_UP )
      continue;
    if ( !_Per_CPU_Information[i].dispatch_necessary )
      continue;
    _SMP_Send_message( i, RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY );
  }
}

/*
 *  Send message to other cores requesting them to shutdown.
 */
void _SMP_Request_other_cores_to_shutdown(void)
{
  bool   allDown;
  int    ncpus;
  int    n;
  int    cpu;

  cpu   = bsp_smp_processor_id();
  ncpus = _SMP_Processor_count;

  _SMP_Broadcast_message( RTEMS_BSP_SMP_SHUTDOWN );

  allDown = true;
  for (n=0 ; n<ncpus ; n++ ) {
     if ( n == cpu ) 
       continue;
     bsp_smp_wait_for(
       (unsigned int *)&_Per_CPU_Information[n].state,
       RTEMS_BSP_SMP_CPU_SHUTDOWN,
       10000
    );
    if ( _Per_CPU_Information[n].state != RTEMS_BSP_SMP_CPU_SHUTDOWN )
      allDown = false;
  }
  if ( !allDown )
    printk( "not all down -- timed out\n" );
  #if defined(RTEMS_DEBUG)
    else
      printk( "All CPUs shutdown successfully\n" );
  #endif
}
