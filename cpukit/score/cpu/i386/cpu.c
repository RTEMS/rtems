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

#include <inttypes.h>

#include <rtems.h>
#include <rtems/score/isr.h>
#include <rtems/score/idtr.h>
#include <rtems/score/tls.h>

#include <rtems/bspIo.h>
#include <rtems/score/percpu.h>
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

RTEMS_STATIC_ASSERT(
  offsetof(Context_Control, gs)
    == I386_CONTEXT_CONTROL_GS_0_OFFSET,
  Context_Control_gs_0
);

#ifdef RTEMS_SMP
  I386_ASSERT_OFFSET(is_executing, IS_EXECUTING);
#endif

#if CPU_HARDWARE_FP
Context_Control_fp _CPU_Null_fp_context;
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
      printk("PANIC: RTEMS was compiled for SSE but BSP did not enable it"
             "(CR4: 0%" PRIu32 ")\n", cr4);
      while ( 1 ) {
        __asm__ __volatile__("hlt");
	  }
	}
  }
#endif
}

/*
 * Stack alignment note:
 *
 * We want the stack to look to the '_entry_point' routine
 * like an ordinary stack frame as if '_entry_point' was
 * called from C-code.
 * Note that '_entry_point' is jumped-to by the 'ret'
 * instruction returning from _CPU_Context_switch() or
 * _CPU_Context_restore() thus popping the _entry_point
 * from the stack.
 * However, _entry_point expects a frame to look like this:
 *
 *      args        [_Thread_Handler expects no args, however]
 *      ------      (alignment boundary)
 * SP-> return_addr return here when _entry_point returns which (never happens)
 *
 *
 * Hence we must initialize the stack as follows
 *
 *         [arg1          ]:  n/a
 *         [arg0 (aligned)]:  n/a
 *         [ret. addr     ]:  NULL
 * SP->    [jump-target   ]:  _entry_point
 *
 * When Context_switch returns it pops the _entry_point from
 * the stack which then finds a standard layout.
 */

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *_stack_base,
  size_t _size,
  uint32_t _isr,
  void (*_entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  uint32_t _stack;
  uint32_t tcb;

  (void) is_fp; /* avoid warning for being unused */

  if ( _isr ) {
    the_context->eflags = CPU_EFLAGS_INTERRUPTS_OFF;
  } else {
    the_context->eflags = CPU_EFLAGS_INTERRUPTS_ON;
  }

  _stack  = ((uint32_t)(_stack_base)) + (_size);
  _stack &= ~ (CPU_STACK_ALIGNMENT - 1);
  _stack -= 2*sizeof(void *); /* see above for why we need to do this */
  *((void (**)(void))(_stack)) = (_entry_point);
  the_context->ebp     = (void *) 0;
  the_context->esp     = (void *) _stack;

  if ( tls_area != NULL ) {
    tcb = (uint32_t) _TLS_TCB_after_TLS_block_initialize( tls_area );
  } else {
    tcb = 0;
  }

  the_context->gs.limit_15_0 = 0xffff;
  the_context->gs.base_address_15_0 = (tcb >> 0) & 0xffff;
  the_context->gs.type = 0x2;
  the_context->gs.descriptor_type = 0x1;
  the_context->gs.limit_19_16 = 0xf;
  the_context->gs.present = 0x1;
  the_context->gs.operation_size = 0x1;
  the_context->gs.granularity = 0x1;
  the_context->gs.base_address_23_16 = (tcb >> 16) & 0xff;
  the_context->gs.base_address_31_24 = (tcb >> 24) & 0xff;
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

#if !defined(I386_DISABLE_INLINE_ISR_DISABLE_ENABLE)
  i386_get_interrupt_level( level );
#else
  level = i386_get_interrupt_level();
#endif

  return level;
}

struct Frame_ {
	struct Frame_  *up;
	uintptr_t		pc;
};

void _CPU_Exception_frame_print (const CPU_Exception_frame *ctx)
{
  unsigned int faultAddr = 0;
  printk("----------------------------------------------------------\n");
  printk("Exception %" PRIu32 " caught at PC %" PRIx32 " by thread %" PRId32 "\n",
	 ctx->idtIndex,
	 ctx->eip,
	 _Thread_Executing->Object.id);
  printk("----------------------------------------------------------\n");
  printk("Processor execution context at time of the fault was  :\n");
  printk("----------------------------------------------------------\n");
  printk(" EAX = %" PRIx32 "	EBX = %" PRIx32 "	ECX = %" PRIx32 "	EDX = %" PRIx32 "\n",
	 ctx->eax, ctx->ebx, ctx->ecx, ctx->edx);
  printk(" ESI = %" PRIx32 "	EDI = %" PRIx32 "	EBP = %" PRIx32 "	ESP = %" PRIx32 "\n",
	 ctx->esi, ctx->edi, ctx->ebp, ctx->esp0);
  printk("----------------------------------------------------------\n");
  printk("Error code pushed by processor itself (if not 0) = %" PRIx32 "\n",
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
			printk("0x%08" PRIx32 " ",fp->pc);
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
