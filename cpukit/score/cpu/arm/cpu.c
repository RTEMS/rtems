/*
 *  ARM CPU Dependent Source
 *
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc
 *      Jay Monkman <jmonkman@adventnetworks.com>
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
 *    thread_dispatch - address of ISR disptaching routine (unused)
 *    
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
    _CPU_Table = *cpu_table;
}

/*
 *
 *  _CPU_ISR_Get_level - returns the current interrupt level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
    unsigned32 reg;
    
    asm volatile ("mrs  %0, cpsr \n"           \
                  "and  %0,  %0, #0xc0 \n"     \
                  : "=r" (reg));
    
    return reg;
}

/*
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    new_handler - replacement ISR for this vector number
 *    old_handler - pointer to store former ISR for this vector number
 *
 *  FIXME: This vector scheme should be changed to allow FIQ to be 
 *         handled better. I'd like to be able to put VectorTable 
 *         elsewhere - JTM
 *
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
    
    if (old_handler != NULL) {
        old_handler = *(proc_ptr *)(VectorTable + vector);
    }

    *(VectorTable + vector) = (long)new_handler ;
  
}

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  unsigned32       *stack_base,
  unsigned32        size,
  unsigned32        new_level,
  void             *entry_point,
  boolean           is_fp
)
{
    the_context->register_sp = ((unsigned32)(stack_base)) + (size) ;
    the_context->register_pc = (entry_point);
    the_context->register_cpsr = (new_level | 0x13);
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
/* FIXME: do something here */
#if 0
    extern unsigned long _fiq_stack;
    extern unsigned long _fiq_stack_size;
    extern unsigned long _irq_stack;
    extern unsigned long _irq_stack_size;
    extern unsigned long _abt_stack;
    extern unsigned long _abt_stack_size;
    unsigned long *ptr;
    int i;

    ptr = &_fiq_stack;
    for (i = 0; i < ((int)&_fiq_stack_size/4); i++) {
        ptr[i] = 0x13131313;
    }

    ptr = &_irq_stack;
    for (i = 0; i < ((int)&_irq_stack_size/4); i++) {
        ptr[i] = 0xf0f0f0f0;
    }

    ptr = &_abt_stack;
    for (i = 0; i < ((int)&_abt_stack_size/4); i++) {
        ptr[i] = 0x55555555;
    }
#endif
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

    while(1); /* FIXME: finish this */
    /* insert your "halt" instruction here */ ;
}

void _defaultExcHandler (CPU_Exception_frame *ctx)
{
    printk("\n\r");
    printk("----------------------------------------------------------\n\r");
    printk("Exception 0x%x caught at PC 0x%x by thread %d\n",
           ctx->register_pc, ctx->register_lr - 4,
           _Thread_Executing->Object.id);
    printk("----------------------------------------------------------\n\r");
    printk("Processor execution context at time of the fault was  :\n\r");
    printk("----------------------------------------------------------\n\r");
    printk(" r0  = %8x  r1  = %8x  r2  = %8x  r3  = %8x\n\r",
           ctx->register_r0, ctx->register_r1, 
           ctx->register_r2, ctx->register_r3);
    printk(" r4  = %8x  r5  = %8x  r6  = %8x  r7  = %8x\n\r",
           ctx->register_r4, ctx->register_r5, 
           ctx->register_r6, ctx->register_r7);
    printk(" r8  = %8x  r9  = %8x  r10 = %8x\n\r",
           ctx->register_r8, ctx->register_r9, ctx->register_r10);
    printk(" fp  = %8x  ip  = %8x  sp  = %8x  pc  = %8x\n\r",
           ctx->register_fp, ctx->register_ip, 
           ctx->register_sp, ctx->register_lr - 4);
    printk("----------------------------------------------------------\n\r");
    
    if (_ISR_Nest_level > 0) {
        /*
         * In this case we shall not delete the task interrupted as
         * it has nothing to do with the fault. We cannot return either
         * because the eip points to the faulty instruction so...
         */
        printk("Exception while executing ISR!!!. System locked\n\r");
        while(1);
    }
    else {
        printk("*********** FAULTY THREAD WILL BE DELETED **************\n\r");
        rtems_task_delete(_Thread_Executing->Object.id);
    }
}

cpuExcHandlerType _currentExcHandler = _defaultExcHandler;

extern void _Exception_Handler_Undef_Swi(); 
extern void _Exception_Handler_Abort(); 
/* FIXME: put comments here */
void rtems_exception_init_mngt()
{
        ISR_Level level;
      
      _CPU_ISR_Disable(level);
      _CPU_ISR_install_vector(ARM_EXCEPTION_UNDEF,
                              _Exception_Handler_Undef_Swi,
                              NULL);
 
      _CPU_ISR_install_vector(ARM_EXCEPTION_SWI,
                              _Exception_Handler_Undef_Swi,
                              NULL);
      
      _CPU_ISR_install_vector(ARM_EXCEPTION_PREF_ABORT,
                              _Exception_Handler_Abort,
                              NULL);
      
      _CPU_ISR_install_vector(ARM_EXCEPTION_DATA_ABORT,
                              _Exception_Handler_Abort,
                              NULL);
      
      _CPU_ISR_install_vector(ARM_EXCEPTION_FIQ,        
                              _Exception_Handler_Abort,
                              NULL);
     
      _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, 
                              _Exception_Handler_Abort,
                              NULL);
     
      _CPU_ISR_Enable(level);
}
  

