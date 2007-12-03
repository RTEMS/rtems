/*  cpu.h
 *
 *  This include file contains information pertaining to the PowerPC
 *  processor.
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
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_OLD_EXCEPTIONS_CPU_H
#define _RTEMS_OLD_EXCEPTIONS_CPU_H

#ifndef _RTEMS_SCORE_CPU_H
#error "You should include <rtems/score/cpu.h>"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM
struct CPU_Interrupt_frame;
typedef void ( *ppc_isr_entry )( int, struct CPU_Interrupt_frame * );
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

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE

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

/*
 *  ACB: This is a lie, but it gets us a handle on a call to set up
 *  a variable derived from the top of the interrupt stack.
 */

#define CPU_HAS_HARDWARE_INTERRUPT_STACK TRUE

/*
 *  Does RTEMS allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 *  If TRUE, then the memory is allocated during initialization.
 *  If FALSE, then the memory is allocated during initialization.
 *
 *  This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 1

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
 */
/*
 *  ACB Note:  This could make debugging tricky..
 */

#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 *  The interrupt level is bit mapped for the PowerPC family. The
 *  bits are set to 0 to indicate that a particular exception source
 *  enabled and 1 if it is disabled.  This keeps with RTEMS convention
 *  that interrupt level 0 means all sources are enabled.
 *
 *  The bits are assigned to correspond to enable bits in the MSR.
 */

#define PPC_INTERRUPT_LEVEL_ME   0x01
#define PPC_INTERRUPT_LEVEL_EE   0x02
#define PPC_INTERRUPT_LEVEL_CE   0x04

/* XXX should these be maskable? */
#if 0
#define PPC_INTERRUPT_LEVEL_DE   0x08
#define PPC_INTERRUPT_LEVEL_BE   0x10
#define PPC_INTERRUPT_LEVEL_SE   0x20
#endif

#define CPU_MODES_INTERRUPT_MASK   0x00000007

/*
 *  Processor defined structures required for cpukit/score.
 */

#ifndef ASM
/*
 *  The following table contains the information required to configure
 *  the PowerPC processor specific parameters.
 */

typedef struct {
  uint32_t     interrupt_stack_size;
}   rtems_cpu_table;
#endif

/*
 *  The following type defines an entry in the PPC's trap table.
 *
 *  NOTE: The instructions chosen are RTEMS dependent although one is
 *        obligated to use two of the four instructions to perform a
 *        long jump.  The other instructions load one register with the
 *        trap type (a.k.a. vector) and another with the psr.
 */
 
#ifndef ASM
typedef struct {
  uint32_t     stwu_r1;                       /* stwu  %r1, -(??+IP_END)(%1)*/
  uint32_t     stw_r0;                        /* stw   %r0, IP_0(%r1)       */
  uint32_t     li_r0_IRQ;                     /* li    %r0, _IRQ            */
  uint32_t     b_Handler;                     /* b     PROC (_ISR_Handler)  */
} CPU_Trap_table_entry;
#endif

/*
 *  This variable is optional.  It is used on CPUs on which it is difficult
 *  to generate an "uninitialized" FP context.  It is filled in by
 *  _CPU_Initialize and copied into the task's FP context area during
 *  _CPU_Context_Initialize.
 */

#ifndef ASM
/* EXTERN Context_Control_fp  _CPU_Null_fp_context; */
#endif

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

#ifndef ASM
SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;
#endif

/*
 *  With some compilation systems, it is difficult if not impossible to
 *  call a high-level language routine from assembly language.  This
 *  is especially true of commercial Ada compilers and name mangling
 *  C++ ones.  This variable can be optionally defined by the CPU porter
 *  and contains the address of the routine _Thread_Dispatch.  This
 *  can make it easier to invoke that routine at the end of the interrupt
 *  sequence (if a dispatch is necessary).
 */

#ifndef ASM
/* EXTERN void           (*_CPU_Thread_dispatch_pointer)(); */
#endif

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 */


#ifndef ASM
SCORE_EXTERN struct {
  uint32_t   volatile* Nest_level;
  uint32_t   volatile* Disable_level;
  void *Vector_table;
  void *Stack;
  uint32_t   Default_r2;
  uint32_t   Default_r13;
  volatile boolean *Switch_necessary;
  boolean *Signal;

  uint32_t   msr_initial;
} _CPU_IRQ_info CPU_STRUCTURE_ALIGNMENT;
#endif

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

#define CPU_INTERRUPT_NUMBER_OF_VECTORS     (PPC_INTERRUPT_MAX)
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (PPC_INTERRUPT_MAX - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS TRUE

/*
 *  ISR handler macros
 */

#ifndef ASM
void _CPU_Initialize_vectors(void);
#endif

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _isr_cookie.
 */

#ifndef ASM
extern const unsigned int _PPC_MSR_DISABLE_MASK;

#define _CPU_MSR_GET( _msr_value ) \
  do { \
    _msr_value = 0; \
    asm volatile ("mfmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

/* FIXME: Backward compatibility
 * new-exception-processing uses _CPU_MSR_GET
 * old-exception-processing had used _CPU_MSR_Value
 */
#define _CPU_MSR_Value(_msr_value) _CPU_MSR_GET(_msr_value)

#define _CPU_MSR_SET( _msr_value ) \
{ asm volatile ("mtmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

#if 0
#define _CPU_ISR_Disable( _isr_cookie ) \
  { register unsigned int _disable_mask = _PPC_MSR_DISABLE_MASK; \
    _isr_cookie = 0; \
    asm volatile ( 
	"mfmsr %0" : \
	"=r" ((_isr_cookie)) : \
	"0" ((_isr_cookie)) \
    ); \
    asm volatile ( 
        "andc %1,%0,%1" : \
	"=r" ((_isr_cookie)), "=&r" ((_disable_mask)) : \
	"0" ((_isr_cookie)), "1" ((_disable_mask)) \
    ); \
    asm volatile ( 
        "mtmsr %1" : \
	"=r" ((_disable_mask)) : \
	"0" ((_disable_mask)) \
    ); \
  }
#endif

#define _CPU_ISR_Disable( _isr_cookie ) \
  { register unsigned int _disable_mask = _PPC_MSR_DISABLE_MASK; \
    _isr_cookie = 0; \
    asm volatile ( \
	"mfmsr %0; andc %1,%0,%1; mtmsr %1" : \
	"=&r" ((_isr_cookie)), "=&r" ((_disable_mask)) : \
	"0" ((_isr_cookie)), "1" ((_disable_mask)) \
	); \
  }
#endif

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _isr_cookie is not modified.
 */

#ifndef ASM
#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
     asm volatile ( "mtmsr %0" : \
		   "=r" ((_isr_cookie)) : \
                   "0" ((_isr_cookie))); \
  }
#endif

/*
 *  This temporarily restores the interrupt to _isr_cookie before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _isr_cookie is not
 *  modified.
 *
 *  NOTE:  The version being used is not very optimized but it does
 *         not trip a problem in gcc where the disable mask does not
 *         get loaded.  Check this for future (post 10/97 gcc versions.
 */

#ifndef ASM
#define _CPU_ISR_Flash( _isr_cookie ) \
  { register unsigned int _disable_mask = _PPC_MSR_DISABLE_MASK; \
    asm volatile ( \
      "mtmsr %0; andc %1,%0,%1; mtmsr %1" : \
      "=&r" ((_isr_cookie)), "=&r" ((_disable_mask)) : \
      "0" ((_isr_cookie)), "1" ((_disable_mask)) \
    ); \
  }
#endif

/*
 *  Map interrupt level in task mode onto the hardware that the CPU
 *  actually provides.  Currently, interrupt levels which do not
 *  map onto the CPU in a generic fashion are undefined.  Someday,
 *  it would be nice if these were "mapped" by the application
 *  via a callout.  For example, m68k has 8 levels 0 - 7, levels
 *  8 - 255 would be available for bsp/application specific meaning.
 *  This could be used to manage a programmable interrupt controller
 *  via the rtems_task_mode directive.
 */

#ifndef ASM
uint32_t   _CPU_ISR_Calculate_level(
  uint32_t   new_level
);

void _CPU_ISR_Set_level(
  uint32_t   new_level
);
  
uint32_t   _CPU_ISR_Get_level( void );

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);
#endif

/* end of ISR handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 */

#define _CPU_Fatal_halt( _error ) \
  _CPU_Fatal_error(_error)

/* end of Fatal Error manager macros */


#ifdef __cplusplus
}
#endif

#endif
