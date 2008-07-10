/*  cpu.h
 *
 *  This include file contains information pertaining to the PowerPC
 *  processor.
 *
 *  Modified for MPC8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited (SSTL), 2001
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/cpu.h:
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be found in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_NEW_EXCEPTIONS_CPU_H
#define _RTEMS_NEW_EXCEPTIONS_CPU_H

#ifndef _RTEMS_SCORE_CPU_H
#error "You should include <rtems/score/cpu.h>"
#endif

#include <rtems/powerpc/registers.h>

#ifdef __cplusplus
extern "C" {
#endif

/* conditional compilation parameters */

/*
 *  Does RTEMS manage a dedicated interrupt stack in software?
 *
 *  If TRUE, then a stack is allocated in _ISR_Handler_initialization.
 *  If FALSE, nothing is done.
 *
 *  If the CPU supports a dedicated interrupt stack in hardware,
 *  then it is generally the responsibility of the BSP to allocate it
 *  and set it up.
 *
 *  If the CPU does not support a dedicated interrupt stack, then
 *  the porter has two options: (1) execute interrupts on the
 *  stack of the interrupted task, and (2) have RTEMS manage a dedicated
 *  interrupt stack.
 *
 *  If this is TRUE, CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 *  Only one of CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 *  CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 *  possible that both are FALSE for a particular CPU.  Although it
 *  is unclear what that would imply about the interrupt processing
 *  procedure on that CPU.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE

/*
 *  Does this CPU have hardware support for a dedicated interrupt stack?
 *
 *  If TRUE, then it must be installed during initialization.
 *  If FALSE, then no installation is performed.
 *
 *  If this is TRUE, CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 *  Only one of CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 *  CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 *  possible that both are FALSE for a particular CPU.  Although it
 *  is unclear what that would imply about the interrupt processing
 *  procedure on that CPU.
 */

#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE

/*
 *  Does RTEMS allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 *  If TRUE, then the memory is allocated during initialization.
 *  If FALSE, then the memory is allocated during initialization.
 *
 *  This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

/*
 *  Should the saving of the floating point registers be deferred
 *  until a context switch is made to another different floating point
 *  task?
 *
 *  If TRUE, then the floating point context will not be stored until
 *  necessary.  It will remain in the floating point registers and not
 *  disturned until another floating point task is switched to.
 *
 *  If FALSE, then the floating point context is saved when a floating
 *  point task is switched out and restored when the next floating point
 *  task is restored.  The state of the floating point registers between
 *  those two operations is not specified.
 *
 *  If the floating point context does NOT have to be saved as part of
 *  interrupt dispatching, then it should be safe to set this to TRUE.
 *
 *  Setting this flag to TRUE results in using a different algorithm
 *  for deciding when to save and restore the floating point context.
 *  The deferred FP switch algorithm minimizes the number of times
 *  the FP context is saved and restored.  The FP context is not saved
 *  until a context switch is made to another, different FP task.
 *  Thus in a system with only one FP task, the FP context will never
 *  be saved or restored.
 *
 *  Note, however that compilers may use floating point registers/
 *  instructions for optimization or they may save/restore FP registers
 *  on the stack. You must not use deferred switching in these cases
 *  and on the PowerPC attempting to do so will raise a "FP unavailable"
 *  exception.
 */
/*
 *  ACB Note:  This could make debugging tricky..
 */

/* conservative setting (FALSE); probably doesn't affect performance too much */
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE

/*
 *  Processor defined structures required for cpukit/score.
 */

#ifndef ASM
  
/*
 *  This variable is optional.  It is used on CPUs on which it is difficult
 *  to generate an "uninitialized" FP context.  It is filled in by
 *  _CPU_Initialize and copied into the task's FP context area during
 *  _CPU_Context_Initialize.
 */

/* EXTERN Context_Control_fp  _CPU_Null_fp_context; */

/*
 *  On some CPUs, RTEMS supports a software managed interrupt stack.
 *  This stack is allocated by the Interrupt Manager and the switch
 *  is performed in _ISR_Handler.  These variables contain pointers
 *  to the lowest and highest addresses in the chunk of memory allocated
 *  for the interrupt stack.  Since it is unknown whether the stack
 *  grows up or down (in general), this give the CPU dependent
 *  code the option of picking the version it wants to use.
 *
 *  NOTE: These two variables are required if the macro
 *        CPU_HAS_SOFTWARE_INTERRUPT_STACK is defined as TRUE.
 */

SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;

#endif /* ndef ASM */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000001 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000001 /* interrupt level in mode */

/*
 *  With some compilation systems, it is difficult if not impossible to
 *  call a high-level language routine from assembly language.  This
 *  is especially true of commercial Ada compilers and name mangling
 *  C++ ones.  This variable can be optionally defined by the CPU porter
 *  and contains the address of the routine _Thread_Dispatch.  This
 *  can make it easier to invoke that routine at the end of the interrupt
 *  sequence (if a dispatch is necessary).
 */

/* EXTERN void           (*_CPU_Thread_dispatch_pointer)(); */

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 */

#ifndef ASM
  
SCORE_EXTERN struct {
  uint32_t   *Disable_level;
  void *Stack;
  volatile boolean *Switch_necessary;
  boolean *Signal;

} _CPU_IRQ_info CPU_STRUCTURE_ALIGNMENT;

#endif /* ndef ASM */

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/*
 * (Optional) # of bytes for libmisc/stackchk to check
 * If not specifed, then it defaults to something reasonable
 * for most architectures.
 */

#define CPU_STACK_CHECK_SIZE    (128)

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by RTEMS.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS     (0)
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (-1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level. Note that
 *  this is not an option - RTEMS/score _relies_ on _ISR_Nest_level
 *  being maintained (e.g. watchdog queues).
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  ISR handler macros
 */

#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _isr_cookie.
 */

#ifndef ASM
  
static inline uint32_t   _CPU_ISR_Get_level( void )
{
  register unsigned int msr;
  _CPU_MSR_GET(msr);
  if (msr & MSR_EE) return 0;
  else	return 1;
}

static inline void _CPU_ISR_Set_level( uint32_t   level )
{
  register unsigned int msr;
  _CPU_MSR_GET(msr);
  if (!(level & CPU_MODES_INTERRUPT_MASK)) {
    msr |= MSR_EE;
  }
  else {
    msr &= ~MSR_EE;
  }
  _CPU_MSR_SET(msr);
}
  
void BSP_panic(char *);

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 */

void _BSP_Fatal_error(unsigned int);

#define _CPU_Fatal_halt( _error ) \
  _BSP_Fatal_error(_error)

/* end of Fatal Error manager macros */

/*
 *  Until all new-exception processing BSPs have fixed
 *  PR288, we let the good BSPs pass
 *
 *  PPC_BSP_HAS_FIXED_PR288
 *
 *  in SPRG0 and let _CPU_Initialize assert this.
 */

#define PPC_BSP_HAS_FIXED_PR288	0x600dbabe

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
