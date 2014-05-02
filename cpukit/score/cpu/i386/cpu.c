/**
 *  @file
 *
 *  @brief Intel i386 Dependent Source
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/types.h>
#include <rtems/score/isr.h>
#include <rtems/score/idtr.h>

#include <rtems/bspIo.h>
#include <rtems/score/thread.h>

#define I386_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control, field) \
      == I386_CONTEXT_CONTROL_ ## off ## _OFFSET, \
    Context_Control_ ## field \
  )

I386_ASSERT_OFFSET(eflags, EFLAGS);
I386_ASSERT_OFFSET(esp, ESP);
I386_ASSERT_OFFSET(ebp, EBP);
I386_ASSERT_OFFSET(ebx, EBX);
I386_ASSERT_OFFSET(esi, ESI);
I386_ASSERT_OFFSET(edi, EDI);

#ifdef RTEMS_SMP
  I386_ASSERT_OFFSET(is_executing, IS_EXECUTING);
#endif

void _CPU_Initialize(void)
{
#if CPU_HARDWARE_FP
  register uint16_t		fp_status __asm__ ("ax");
  register Context_Control_fp  *fp_context;
#endif

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
  __asm__ volatile( "fninit" );
  __asm__ volatile( "fnstsw %0" : "=a" (fp_status) : "0" (fp_status) );

  if ( fp_status ==  0 ) {

    fp_context = &_CPU_Null_fp_context;

#ifdef __SSE__
	asm volatile( "fstcw %0":"=m"(fp_context->fpucw) );
#else
    __asm__ volatile( "fsave (%0)" : "=r" (fp_context)
                               : "0"  (fp_context)
                );
#endif
  }
#endif

#ifdef __SSE__

  __asm__ volatile("stmxcsr %0":"=m"(fp_context->mxcsr));

  /* The BSP must enable the SSE extensions (early).
   * If any SSE instruction was already attempted
   * then that crashed the system.
   * As a courtesy, we double-check here but it
   * may be too late (which is also why we don't
   * enable SSE here).
   */
  {
  uint32_t cr4;
    __asm__ __volatile__("mov %%cr4, %0":"=r"(cr4));
    if ( 0x600 != (cr4 & 0x600) ) {
      printk("PANIC: RTEMS was compiled for SSE but BSP did not enable it (CR4: 0x%08x)\n", cr4);
      while ( 1 ) {
        __asm__ __volatile__("hlt");
	  }
	}
  }
#endif
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

  i386_get_interrupt_level( level );

  return level;
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  while(1){
    __asm__ volatile ("hlt");
  }
  return NULL;
}

struct Frame_ {
	struct Frame_  *up;
	uintptr_t		pc;
};

void _CPU_Exception_frame_print (const CPU_Exception_frame *ctx)
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
  }
  else {
  	struct Frame_ *fp = (struct Frame_*)ctx->ebp;
	int           i;

	printk("Call Stack Trace of EIP:\n");
	if ( fp ) {
		for ( i=1; fp->up; fp=fp->up, i++ ) {
			printk("0x%08x ",fp->pc);
			if ( ! (i&3) )
				printk("\n");
		}
	}
	printk("\n");
    /*
     * OK I could probably use a simplified version but at least this
     * should work.
     */
#if 0
    printk(" ************ FAULTY THREAD WILL BE SUSPENDED **************\n");
    rtems_task_suspend(_Thread_Executing->Object.id);
#endif
  }
}

static void _defaultExcHandler (CPU_Exception_frame *ctx)
{
  rtems_fatal(
    RTEMS_FATAL_SOURCE_EXCEPTION,
    (rtems_fatal_code) ctx
  );
}

cpuExcHandlerType _currentExcHandler = _defaultExcHandler;

extern void rtems_exception_prologue_0(void);
extern void rtems_exception_prologue_1(void);
extern void rtems_exception_prologue_2(void);
extern void rtems_exception_prologue_3(void);
extern void rtems_exception_prologue_4(void);
extern void rtems_exception_prologue_5(void);
extern void rtems_exception_prologue_6(void);
extern void rtems_exception_prologue_7(void);
extern void rtems_exception_prologue_8(void);
extern void rtems_exception_prologue_9(void);
extern void rtems_exception_prologue_10(void);
extern void rtems_exception_prologue_11(void);
extern void rtems_exception_prologue_12(void);
extern void rtems_exception_prologue_13(void);
extern void rtems_exception_prologue_14(void);
extern void rtems_exception_prologue_16(void);
extern void rtems_exception_prologue_17(void);
extern void rtems_exception_prologue_18(void);
#ifdef __SSE__
extern void rtems_exception_prologue_19(void);
#endif

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
     0,
	 rtems_exception_prologue_16,
	 rtems_exception_prologue_17,
	 rtems_exception_prologue_18,
#ifdef __SSE__
	 rtems_exception_prologue_19,
#endif
};

void rtems_exception_init_mngt(void)
{
      size_t                     i,j;
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
