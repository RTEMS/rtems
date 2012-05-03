/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM exception support implementation.
 */

/*
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
 *  Moved from file 'cpukit/score/cpu/arm/cpu.c'.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>
#include <rtems/score/cpu.h>

#ifdef ARM_MULTILIB_ARCH_V4

static void _defaultExcHandler (CPU_Exception_frame *ctx)
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

typedef void (*cpuExcHandlerType) (CPU_Exception_frame*);

cpuExcHandlerType _currentExcHandler = _defaultExcHandler;

extern void _Exception_Handler_Undef_Swi(void);
extern void _Exception_Handler_Abort(void);
extern void _exc_data_abort(void);



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

#endif /* ARM_MULTILIB_ARCH_V4 */
