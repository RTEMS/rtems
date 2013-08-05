/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM architecture support implementation.
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
 *  Copyright (c) 2009-2011 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#ifdef ARM_MULTILIB_VFP_D32
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, register_d8 ) == ARM_CONTEXT_CONTROL_D8_OFFSET,
    ARM_CONTEXT_CONTROL_D8_OFFSET
  );
#endif

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) == ARM_EXCEPTION_FRAME_SIZE,
  ARM_EXCEPTION_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_sp )
    == ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET,
  ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET
);

RTEMS_STATIC_ASSERT(
  sizeof( ARM_VFP_context ) == ARM_VFP_CONTEXT_SIZE,
  ARM_VFP_CONTEXT_SIZE
);

#ifdef ARM_MULTILIB_ARCH_V4

/*
 * This variable can be used to change the running mode of the execution
 * contexts.
 */
uint32_t arm_cpu_mode = 0x13;

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp
)
{
  the_context->register_sp = (uint32_t) stack_area_begin + stack_area_size;
  the_context->register_lr = (uint32_t) entry_point;
  the_context->register_cpsr = ( ( new_level != 0 ) ? ARM_PSR_I : 0 )
    | arm_cpu_mode;
}

/* Preprocessor magic for stringification of x */
#define _CPU_ISR_LEVEL_DO_STRINGOF( x) #x
#define _CPU_ISR_LEVEL_STRINGOF( x) _CPU_ISR_LEVEL_DO_STRINGOF( x)

void _CPU_ISR_Set_level( uint32_t level )
{
  uint32_t arm_switch_reg;

  level = ( level != 0 ) ? ARM_PSR_I : 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[arm_switch_reg], cpsr\n"
    "bic %[arm_switch_reg], #" _CPU_ISR_LEVEL_STRINGOF( ARM_PSR_I ) "\n"
    "orr %[arm_switch_reg], %[level]\n"
    "msr cpsr, %0\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg)
    : [level] "r" (level)
  );
}

uint32_t _CPU_ISR_Get_level( void )
{
  ARM_SWITCH_REGISTERS;
  uint32_t level;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[level], cpsr\n"
    "and %[level], #" _CPU_ISR_LEVEL_STRINGOF( ARM_PSR_I ) "\n"
    ARM_SWITCH_BACK
    : [level] "=&r" (level) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return ( level & ARM_PSR_I ) != 0;
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

void _CPU_Initialize( void )
{
  /* Do nothing */
}

#endif /* ARM_MULTILIB_ARCH_V4 */
