/*
 *  SPARC-v9 Dependent Source
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This file is based on the SPARC cpu.c file. Modifications are made to 
 *  provide support for the SPARC-v9.
 *    COPYRIGHT (c) 2010. Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/asm.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/cache.h>

/*
 *  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 *
 *  Output Parameters: NONE
 *
 *  NOTE: There is no need to save the pointer to the thread dispatch routine.
 *        The SPARC's assembly code can reference it directly with no problems.
 */

void _CPU_Initialize(void)
{
#if (SPARC_HAS_FPU == 1)
  Context_Control_fp *pointer;

  /*
   *  This seems to be the most appropriate way to obtain an initial
   *  FP context on the SPARC.  The NULL fp context is copied in to
   *  the task's FP context during Context_Initialize_fp.
   */

  pointer = &_CPU_Null_fp_context;
  _CPU_Context_save_fp( &pointer );

#endif

  /*
   *  Since no tasks have been created yet and no interrupts have occurred,
   *  there is no way that the currently executing thread can have an
   *  _ISR_Dispatch stack frame on its stack.
   */
  _CPU_ISR_Dispatch_disable = 0;
}

/*
 *  _CPU_Context_Initialize
 *
 *  This kernel routine initializes the basic non-FP context area associated
 *  with each thread.
 *
 *  Input parameters:
 *    the_context  - pointer to the context area
 *    stack_base   - address of memory for the SPARC
 *    size         - size in bytes of the stack area
 *    new_level    - interrupt level for this context area
 *    entry_point  - the starting execution point for this this context
 *    is_fp        - TRUE if this context is associated with an FP thread
 *
 *  Output parameters: NONE
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  void         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
    uint64_t     stack_high;  /* highest "stack aligned" address */
    uint32_t     the_size;

    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the stack_high address.
     */

    stack_high = ((uint64_t)(stack_base) + size);
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    the_size = size & ~(CPU_STACK_ALIGNMENT - 1);

    /*
     *  See the README in this directory for a diagram of the stack.
     */

    the_context->o7    = ((uint64_t) entry_point) - 8;
    the_context->o6_sp = stack_high - CPU_MINIMUM_STACK_FRAME_SIZE - STACK_BIAS;
    the_context->i6_fp = 0;

    /* ABI uses g4 as segment register, make sure it is zeroed */
    the_context->g4    = 0;

    /* PSTATE used to be built here, but is no longer included in context */

  /*
   *  Since THIS thread is being created, there is no way that THIS
   *  thread can have an _ISR_Dispatch stack frame on its stack.
   */
    the_context->isr_dispatch_disable = 0;
}
