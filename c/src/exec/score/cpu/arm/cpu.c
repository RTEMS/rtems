/*
 *  ARM CPU Dependent Source
 *
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

#include <rtems/system.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>
#include <rtems/score/cpu.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */

  return 0;
}

/*
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /* pointer on the redirection table in RAM */
  long *VectorTable = (long *)(MAX_EXCEPTIONS * 4); 

  if (old_handler != NULL)
    old_handler = *(proc_ptr *)(VectorTable + vector);
  *(VectorTable + vector) = (long)new_handler ;
  
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*PAGE
 *
 *  _CPU_Thread_Idle_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

void _CPU_Thread_Idle_body( void )
{

  while(1);
    /* insert your "halt" instruction here */ ;
}

void _defaultExcHandler (CPU_Exception_frame *ctx)
{
  printk("----------------------------------------------------------\n");
  printk("Exception %d caught at PC %x by thread %d\n",
	 ctx->register_pc, ctx->register_lr - 4,
	 _Thread_Executing->Object.id);
  printk("----------------------------------------------------------\n");
  printk("Processor execution context at time of the fault was  :\n");
  printk("----------------------------------------------------------\n");
  printk(" r0 = %x     r1 = %x     r2 = %x     r3 = %x\n",
	 ctx->register_r0, ctx->register_r1, ctx->register_r2, ctx->register_r3);
  printk(" r4 = %x     r5 = %x     r6 = %x     r7 = %x\n",
	 ctx->register_r4, ctx->register_r5, ctx->register_r6, ctx->register_r7);
  printk(" r8 = %x     r9 = %x     r10 = %x\n",
	 ctx->register_r8, ctx->register_r9, ctx->register_r10);
  printk(" fp = %x     ip = %x     sp = %x     pc = %x\n",
	 ctx->register_fp, ctx->register_ip, ctx->register_sp, ctx->register_lr - 4);
  printk("----------------------------------------------------------\n");

 if (_ISR_Nest_level > 0) {
    /*
     * In this case we shall not delete the task interrupted as
     * it has nothing to do with the fault. We cannot return either
     * because the eip points to the faulty instruction so...
     */
    printk("Exception while executing ISR!!!. System locked\n");
    while(1);
  }
 else {
   printk(" ************ FAULTY THREAD WILL BE DELETED **************\n");
   rtems_task_delete(_Thread_Executing->Object.id);
 }
}

cpuExcHandlerType _currentExcHandler = _defaultExcHandler;

extern void _Exception_Handler_Undef_Swi(); 
extern void _Exception_Handler_Abort(); 

void rtems_exception_init_mngt()
{
	ISR_Level level;
      
      _CPU_ISR_Disable(level);
      _CPU_ISR_install_vector(ARM_EXCEPTION_UNDEF,      _Exception_Handler_Undef_Swi, NULL); 
      _CPU_ISR_install_vector(ARM_EXCEPTION_SWI,        _Exception_Handler_Undef_Swi, NULL); 
      _CPU_ISR_install_vector(ARM_EXCEPTION_PREF_ABORT, _Exception_Handler_Abort    , NULL); 
      _CPU_ISR_install_vector(ARM_EXCEPTION_DATA_ABORT, _Exception_Handler_Abort    , NULL);
      _CPU_ISR_install_vector(ARM_EXCEPTION_FIQ, 	_Exception_Handler_Abort    , NULL);
      _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, 	_Exception_Handler_Abort    , NULL);
      _CPU_ISR_Enable(level);
}
  

