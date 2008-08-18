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
 *  Copyright (c) 2007 Ray xu <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>
#include <rtems/score/cpu.h>

/*
 * This variable can be used to change the running mode of the execution
 * contexts.
 */

unsigned int arm_cpu_mode = 0x13;

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    thread_dispatch - address of ISR disptaching routine (unused)
 */

void _CPU_Initialize(
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
}

/*
 *
 *  _CPU_ISR_Get_level - returns the current interrupt level
 */
#define str(x) #x
#define xstr(x) str(x)
#define L(x) #x "_" xstr(__LINE__)

#define TO_ARM_MODE(x)      \
    asm volatile (          \
    ".code  16           \n" \
    L(x) "_thumb:        \n" \
    ".align 2            \n" \
    "push {lr}           \n" \
    "adr %0, "L(x) "_arm \n" \
    "bl " L(x)"         \n" \
    "pop    {pc}        \n" \
    ".balign 4          \n" \
    L(x) ":             \n" \
    "bx %0              \n" \
    "nop                \n" \
    ".pool              \n" \
    ".code 32           \n" \
    L(x) "_arm:         \n" \
    :"=&r" (reg))
 

/*
 * Switch to Thumb mode Veneer,ugly but safe
 */

#define TO_THUMB_MODE(x)    \
    asm volatile (          \
        ".code  32                  \n"\
        "adr %0, "L(x) "_thumb +1   \n"\
        "bx  %0                     \n"\
        ".pool                      \n"\
        ".thumb_func                \n"\
        L(x) "_thumb:               \n"\
        : "=&r" (reg))

#if (!defined(__THUMB_INTERWORK__) &&  !defined(__thumb__)) 
uint32_t   _CPU_ISR_Get_level( void )
{
    uint32_t   reg = 0; /* to avoid warning */
    asm volatile ("mrs  %0, cpsr \n"           \
                  "and  %0,  %0, #0xc0 \n"     \
                  : "=r" (reg)                 \
                  : "0" (reg) );
    return reg;
}
#endif



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
  uint32_t    vector,
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
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  boolean           is_fp
)
{
    the_context->register_sp = (uint32_t  )stack_base + size ;
    the_context->register_lr = (uint32_t  )entry_point;
    the_context->register_cpsr = new_level | arm_cpu_mode;
}


/*
 *  _CPU_Install_interrupt_stack - this function is empty since the
 *  BSP must set up the interrupt stacks.
 */

void _CPU_Install_interrupt_stack( void )
{
}

void _defaultExcHandler (CPU_Exception_frame *ctx)
{
    printk("\n\r");
    printk("----------------------------------------------------------\n\r");
#if 1
    printk("Exception 0x%x caught at PC 0x%x by thread %d\n",
           ctx->register_ip, ctx->register_lr - 4,
           _Thread_Executing->Object.id);
#endif
    printk("----------------------------------------------------------\n\r");
    printk("Processor execution context at time of the fault was  :\n\r");
    printk("----------------------------------------------------------\n\r");
#if 0
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
#endif    
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
extern void _exc_data_abort(); 



/* FIXME: put comments here */
void rtems_exception_init_mngt(void)
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
                              _exc_data_abort,
                              NULL);
      
      _CPU_ISR_install_vector(ARM_EXCEPTION_FIQ,        
                              _Exception_Handler_Abort,
                              NULL);
     
      _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, 
                              _Exception_Handler_Abort,
                              NULL);
     
      _CPU_ISR_Enable(level);
}
  
#define INSN_MASK         0xc5

#define INSN_STM1         0x80
#define INSN_STM2         0x84
#define INSN_STR          0x40
#define INSN_STRB         0x44

#define INSN_LDM1         0x81
#define INSN_LDM23        0x85
#define INSN_LDR          0x41
#define INSN_LDRB         0x45

#define GET_RD(x)         ((x & 0x0000f000) >> 12)
#define GET_RN(x)         ((x & 0x000f0000) >> 16)

#define GET_U(x)              ((x & 0x00800000) >> 23)
#define GET_I(x)              ((x & 0x02000000) >> 25)

#define GET_REG(r, ctx)      (((uint32_t   *)ctx)[r])
#define SET_REG(r, ctx, v)   (((uint32_t   *)ctx)[r] = v)
#define GET_OFFSET(insn)     (insn & 0xfff)

