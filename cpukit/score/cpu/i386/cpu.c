/*
 *  Intel i386 Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/types.h>
#include <rtems/score/isr.h>
#include <rtems/score/idtr.h>

#include <rtems/bspIo.h>
#include <rtems/score/thread.h>

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
#if CPU_HARDWARE_FP
  register unsigned16  fp_status asm ("ax");
  register void       *fp_context;
#endif

  _CPU_Table = *cpu_table;

  /*
   *  The following code saves a NULL i387 context which is given
   *  to each task at start and restart time.  The following code
   *  is based upon that provided in the i386 Programmer's
   *  Manual and should work on any coprocessor greater than
   *  the i80287.
   *
   *  NOTE: The NO WAIT form of the coprocessor instructions
   *        MUST be used in case there is not a coprocessor
   *        to wait for.
   */

#if CPU_HARDWARE_FP
  fp_status = 0xa5a5;
  asm volatile( "fninit" );
  asm volatile( "fnstsw %0" : "=a" (fp_status) : "0" (fp_status) );

  if ( fp_status ==  0 ) {

    fp_context = &_CPU_Null_fp_context;

    asm volatile( "fsave (%0)" : "=r" (fp_context)
                               : "0"  (fp_context)
                );
  }
#endif

}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  i386_get_interrupt_level( level );
 
  return level;
}

void _CPU_Thread_Idle_body ()
{
  while(1){
    asm volatile ("hlt");
  }
}

void _defaultExcHandler (CPU_Exception_frame *ctx)
{
  unsigned int faultAddr = 0;
  printk("----------------------------------------------------------\n");
  printk("Exception %d caught at PC %x by thread %d\n",
	 ctx->idtIndex,
	 ctx->eip,
	 _Thread_Executing->Object.id);
  printk("----------------------------------------------------------\n");
  printk("Processor execution context at time of the fault was  :\n");
  printk("----------------------------------------------------------\n");
  printk(" EAX = %x	EBX = %x	ECX = %x	EDX = %x\n",
	 ctx->eax, ctx->ebx, ctx->ecx, ctx->edx);
  printk(" ESI = %x	EDI = %x	EBP = %x	ESP = %x\n",
	 ctx->esi, ctx->edi, ctx->ebp, ctx->esp0);
  printk("----------------------------------------------------------\n");
  printk("Error code pushed by processor itself (if not 0) = %x\n",
	 ctx->faultCode);
  printk("----------------------------------------------------------\n");
  if (ctx->idtIndex == I386_EXCEPTION_PAGE_FAULT){
    faultAddr = i386_get_cr2();
    printk("Page fault linear address (CR2) = %x\n", faultAddr);
    printk("----------------------------------------------------------\n\n");
  }
 if (_ISR_Nest_level > 0) {
    /*
     * In this case we shall not delete the task interrupted as
     * it has nothing to do with the fault. We cannot return either
     * because the eip points to the faulty instruction so...
     */
    printk("Exception while executing ISR!!!. System locked\n");
    _CPU_Fatal_halt(faultAddr);
  }
  else {
    /*
     * OK I could probably use a simplified version but at least this
     * should work.
     */
    printk(" ************ FAULTY THREAD WILL BE DELETED **************\n");
    rtems_task_delete(_Thread_Executing->Object.id);
  }
}

cpuExcHandlerType _currentExcHandler = _defaultExcHandler;

extern void rtems_exception_prologue_0();
extern void rtems_exception_prologue_1();
extern void rtems_exception_prologue_2();
extern void rtems_exception_prologue_3();
extern void rtems_exception_prologue_4();
extern void rtems_exception_prologue_5();
extern void rtems_exception_prologue_6();
extern void rtems_exception_prologue_7();
extern void rtems_exception_prologue_8();
extern void rtems_exception_prologue_9();
extern void rtems_exception_prologue_10();
extern void rtems_exception_prologue_11();
extern void rtems_exception_prologue_12();
extern void rtems_exception_prologue_13();
extern void rtems_exception_prologue_14();
extern void rtems_exception_prologue_16();
extern void rtems_exception_prologue_17();
extern void rtems_exception_prologue_18();

static rtems_raw_irq_hdl tbl[] = {
	 rtems_exception_prologue_0,
	 rtems_exception_prologue_1,
	 rtems_exception_prologue_2,
	 rtems_exception_prologue_3,
	 rtems_exception_prologue_4,
	 rtems_exception_prologue_5,
	 rtems_exception_prologue_6,
	 rtems_exception_prologue_7,
	 rtems_exception_prologue_8,
	 rtems_exception_prologue_9,
	 rtems_exception_prologue_10,
	 rtems_exception_prologue_11,
	 rtems_exception_prologue_12,
	 rtems_exception_prologue_13,
	 rtems_exception_prologue_14,
	 rtems_exception_prologue_16,
	 rtems_exception_prologue_17,
	 rtems_exception_prologue_18,
};

void rtems_exception_init_mngt()
{
      unsigned int		 i,j;
      interrupt_gate_descriptor	 *currentIdtEntry;
      unsigned			 limit;
      unsigned			 level;
      
      i = sizeof(tbl) / sizeof (rtems_raw_irq_hdl);

      i386_get_info_from_IDTR (&currentIdtEntry, &limit);

      _CPU_ISR_Disable(level);
      for (j = 0; j < i; j++) {
	create_interrupt_gate_descriptor (&currentIdtEntry[j], tbl[j]);
      }
      _CPU_ISR_Enable(level);
}
      
