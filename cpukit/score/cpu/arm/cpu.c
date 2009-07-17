/**
 * @file
 *
 * ARM support code.
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
 *  Copyright (c) 2009 embedded brains GmbH
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
uint32_t arm_cpu_mode = 0x13;

void _CPU_Context_Initialize(
  Context_Control *the_context,
  uint32_t *stack_base,
  uint32_t size,
  uint32_t new_level,
  void *entry_point,
  bool is_fp
)
{
  the_context->register_sp = (uint32_t) stack_base + size ;
  the_context->register_lr = (uint32_t) entry_point;
  the_context->register_cpsr = new_level | arm_cpu_mode;
}

/* Preprocessor magic for stringification of x */
#define _CPU_ISR_LEVEL_DO_STRINGOF( x) #x
#define _CPU_ISR_LEVEL_STRINGOF( x) _CPU_ISR_LEVEL_DO_STRINGOF( x)

void _CPU_ISR_Set_level( uint32_t level )
{
  uint32_t reg;

  asm volatile (
    THUMB_TO_ARM
    "mrs %0, cpsr\n"
    "bic %0, %0, #" _CPU_ISR_LEVEL_STRINGOF( CPU_MODES_INTERRUPT_MASK ) "\n"
    "orr %0, %0, %1\n"
    "msr cpsr, %0\n"
    ARM_TO_THUMB
    : "=r" (reg)
    : "r" (level)
  );
}

uint32_t _CPU_ISR_Get_level( void )
{
  uint32_t reg;
  uint32_t level;

  asm volatile (
    THUMB_TO_ARM
    "mrs %0, cpsr\n"
    "and %1, %0, #" _CPU_ISR_LEVEL_STRINGOF( CPU_MODES_INTERRUPT_MASK ) "\n"
    ARM_TO_THUMB
    : "=r" (reg), "=r" (level)
  );

  return level;
}

void _CPU_ISR_install_vector(
  uint32_t vector,
  proc_ptr new_handler,
  proc_ptr *old_handler
)
{
  /* Redirection table starts at the end of the vector table */
  volatile uint32_t *table = (volatile uint32_t *) (MAX_EXCEPTIONS * 4); 

  uint32_t current_handler = table [vector];
  
  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = (proc_ptr) current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only memory */
  if (current_handler != (uint32_t) new_handler) {
    table [vector] = (uint32_t) new_handler;
  }
}

void _CPU_Install_interrupt_stack( void )
{
  /* This function is empty since the BSP must set up the interrupt stacks */
}

void _CPU_Initialize( void )
{
  /* Do nothing */
}
