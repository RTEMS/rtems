/**
 *  @file cpu.c
 *
 *  ARM CPU Dependent Source.
 */
/*
 *  RTEMS GBA BSP
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *      Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  Copyright (c) 2004
 *      Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <stdint.h>
#include <rtems/system.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>
#include <rtems/score/cpu.h>
#include <arm_mode_bits.h>

/**
 *  @brief _CPU_Initialize routine performs processor dependent initialization
 *
 *  @param  cpu_table CPU table to initialize
 *  @param  thread_dispatch address of ISR disptaching routine (unused)
 *  @return None
 */
void _CPU_Initialize(
  void      (*thread_dispatch)       /* ignored on this CPU */
)
{
}

/**
 *  @brief _CPU_ISR_Get_level returns the current interrupt level
 *
 *  @param  None
 *  @return int level
 */
uint32_t  _CPU_ISR_Get_level( void )
{
    uint32_t  reg = 0; /* to avoid warning */

    asm volatile ("mrs  %0, cpsr \n"        \
                  "and  %0,  %0, #0xc0 \n"  \
                  : "=r" (reg)              \
                  : "0" (reg) );
    return reg;
}


/**
 *  @brief _CPU_ISR_install_vector kernel routine installs the RTEMS handler for the
 *  specified vector
 *
 *  @param  vector interrupt vector number
 *  @param  new_handler replacement ISR for this vector number
 *  @param  old_handler pointer to store former ISR for this vector number
 *  @return None
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 */
extern __inline__ void _CPU_ISR_install_vector(uint32_t vector, proc_ptr new_handler, proc_ptr *old_handler);

/**
 *  @brief _CPU_Context_Initialize kernel routine initialize the specified context
 *
 *  @param  the_context
 *  @param  stack_base
 *  @param  size
 *  @param  new_level
 *  @param  entry_point
 *  @param  is_fp
 *  @return None
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  boolean           is_fp
)
{
    the_context->register_sp = (uint32_t)stack_base + size ;
    the_context->register_lr = (uint32_t)entry_point;
    the_context->register_cpsr = new_level | ModePriv;
}


/**
 *  @brief _CPU_Install_interrupt_stack function is empty since the BSP must set up the interrupt stacks.
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 */
extern __inline__ void  _CPU_Install_interrupt_stack( void );

/**
 *  @brief _defaultExcHandler function is empty
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 */
extern void _defaultExcHandler (CPU_Exception_frame *ctx);

/**
 *  @brief _currentExcHandler function is empty (_defaultExcHandler)
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 */
cpuExcHandlerType _currentExcHandler = _defaultExcHandler;
/*
extern void _Exception_Handler_Undef_Swi();
extern void _Exception_Handler_Abort();
extern void _exc_data_abort();
*/

/**
 *  @brief rtems_exception_init_mngt function is empty since the BSP must set up the interrupt stacks.
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 */
extern __inline__ void rtems_exception_init_mngt();


/**
 *  @brief do_data_abort function is empty
 *
 *  This function figure out what caused the data abort
 *
 *  @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!
 *  This function is supposed to figure out what caused the data abort, do that, then return.
 *  All unhandled instructions cause the system to hang.
 */
extern __inline__ void do_data_abort(uint32_t insn, uint32_t spsr, CPU_Exception_frame *ctx);


/* @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS!!  */
/* @todo Remove dummy functions needed by linker
 ****************************************************************************************/
/* @cond  INCLUDE_ASM */
asm ("  .text");
asm ("  .arm");
asm ("  .global _CPU_ISR_install_vector");
asm ("_CPU_ISR_install_vector:");
asm ("  .global _CPU_Install_interrupt_stack");
asm ("_CPU_Install_interrupt_stack:");
asm ("  .global _defaultExcHandler");
asm ("_defaultExcHandler:");
asm ("  .global rtems_exception_init_mngt");
asm ("rtems_exception_init_mngt:");
asm ("  .global do_data_abort");
asm ("do_data_abort:");
asm ("  mov pc, lr");
/* @endcond */

