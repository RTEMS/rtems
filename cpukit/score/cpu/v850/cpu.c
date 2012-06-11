/**
 *  @file
 *
 *  v850 CPU Dependent Source
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>

#include <string.h> /* for memset */

/*
 *  v850 Specific Information:
 *
 *  So far nothing known to be needed at this point during initialization.
 */
void _CPU_Initialize(void)
{
}

/*
 *  v850 Specific Information:
 *
 *  This method returns 0 if interrupts are enabled and 1 if they are disabled.
 *  The v850 only has two interrupt levels (on and off).
 */
uint32_t _CPU_ISR_Get_level( void )
{
  unsigned int psw;

  v850_get_psw( psw );

  if ( (psw & V850_PSW_INTERRUPT_DISABLE_MASK) == V850_PSW_INTERRUPT_DISABLE )
    return 1;

  return 0;
}

/*
 *  v850 Specific Information:
 *
 *  This method initializes a v850 context control structure.
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
  uint32_t  stack_high;  /* highest "stack aligned" address */
  uint32_t  psw;         /* highest "stack aligned" address */

  memset( the_context, 0, sizeof(Context_Control) );

  /*
   *  On CPUs with stacks which grow down, we build the stack
   *  based on the stack_high address.
   */
  stack_high = ((uint32_t)(stack_base) + size);
  stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

  v850_get_psw( psw );
  psw &= ~V850_PSW_INTERRUPT_DISABLE_MASK;
  if ( new_level )
    psw |= V850_PSW_INTERRUPT_DISABLE;
  else
    psw |= V850_PSW_INTERRUPT_ENABLE;

  the_context->r31              = (uint32_t) entry_point;
  the_context->r3_stack_pointer = stack_high;
  the_context->psw              = psw;

#if 0
  printk( "the_context = %p\n",      the_context );
  printk( "stack base  = 0x%08x\n",  stack_base );
  printk( "stack size  = 0x%08x\n",  size );
  printk( "sp          = 0x%08x\n",  the_context->r3_stack_pointer );
  printk( "psw         = 0x%08x\n",  the_context->psw );
#endif
}

