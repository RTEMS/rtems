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

unsigned32 g_data_abort_cnt = 0;
unsigned32 g_data_abort_insn_list[1024];

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
                  : "=r" (reg)                 \
                  : "0" (reg) );
    
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
    the_context->register_sp = (unsigned32)stack_base + size ;
    the_context->register_lr = (unsigned32)entry_point;
    the_context->register_cpsr = new_level | 0x13;
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
#if 0
    printk("Exception 0x%x caught at PC 0x%x by thread %d\n",
           ctx->register_pc, ctx->register_lr - 4,
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

#define GET_REG(r, ctx)      (((unsigned32 *)ctx)[r])
#define SET_REG(r, ctx, v)   (((unsigned32 *)ctx)[r] = v)
#define GET_OFFSET(insn)     (insn & 0xfff)


/* This function is supposed to figure out what caused the 
 * data abort, do that, then return.
 *
 * All unhandled instructions cause the system to hang.
 */

void do_data_abort(unsigned32 insn, unsigned32 spsr, 
                   CPU_Exception_frame *ctx)
{
    unsigned8  decode;
    unsigned8  insn_type;

    unsigned32 rn;
    unsigned32 rd;

    unsigned8  *src_addr;
    unsigned8  *dest_addr;
    unsigned32  tmp;

    g_data_abort_insn_list[g_data_abort_cnt & 0x3ff] = ctx->register_lr - 8;
    g_data_abort_cnt++;
    
    decode = ((insn >> 20) & 0xff);

    insn_type = decode & INSN_MASK;
    switch(insn_type) {
    case INSN_STM1:
        printk("\n\nINSN_STM1\n");
        break;
    case INSN_STM2:
        printk("\n\nINSN_STM2\n");
        break;
    case INSN_STR:
        printk("\n\nINSN_STR\n");
        break;
    case INSN_STRB:
        printk("\n\nINSN_STRB\n");
        break;
    case INSN_LDM1:
        printk("\n\nINSN_LDM1\n");
        break;
    case INSN_LDM23:
        printk("\n\nINSN_LDM23\n");
        break;
    case INSN_LDR:
        printk("\n\nINSN_LDR\n");

        rn = GET_RN(insn);
        rd = GET_RD(insn);

        /* immediate offset/index */
        if (GET_I(insn) == 0) {
            switch(decode & 0x12) {  /* P and W bits */
            case 0x00:  /* P=0, W=0 -> base is updated, post-indexed */
                printk("\tPost-indexed\n");
                break;
            case 0x02:  /* P=0, W=1 -> user mode access */
                printk("\tUser mode\n");
                break;
            case 0x10:  /* P=1, W=0 -> base not updated */
                src_addr = GET_REG(rn, ctx);
                if (GET_U(insn) == 0) {
                    src_addr -= GET_OFFSET(insn);
                } else {
                    src_addr += GET_OFFSET(insn);
                }
                tmp = (src_addr[0] | 
                       (src_addr[1] << 8) | 
                       (src_addr[2] << 16) | 
                       (src_addr[3] << 24));
                SET_REG(rd, ctx, tmp);
                return;
                break;
            case 0x12:  /* P=1, W=1 -> base is updated, pre-indexed */
                printk("\tPre-indexed\n");
                break;
            }
        }
        break;
    case INSN_LDRB:
        printk("\n\nINSN_LDRB\n");
        break;
    default:
        printk("\n\nUnrecognized instruction\n");
        break;
    }
    
    printk("data_abort at address 0x%x, instruction: 0x%x,   spsr = 0x%x\n",
           ctx->register_lr - 8, insn, spsr);

    /* disable interrupts, wait forever */
    _CPU_ISR_Disable(tmp);
    while(1) {
        continue;
    }
    return;
}


    
