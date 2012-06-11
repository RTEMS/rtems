/**
 * @file rtems/score/cpu.h
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  COPYRIGHT (c) 1995 i-cubed ltd.
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
 *  Copyright (c) 2001 Andy Dachs <a.dachs@sstl.co.uk>.
 *
 *  Copyright (c) 2001 Surrey Satellite Technology Limited (SSTL).
 *
 *  Copyright (c) 2010-2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/types.h>
#include <rtems/score/powerpc.h>
#include <rtems/powerpc/registers.h>

#ifndef ASM
  #include <string.h> /* for memset() */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* conditional compilation parameters */

/*
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  If TRUE, then they are inlined.
 *  If FALSE, then a subroutine call is made.
 *
 *  Basically this is an example of the classic trade-off of size
 *  versus speed.  Inlining the call (TRUE) typically increases the
 *  size of RTEMS while speeding up the enabling of dispatching.
 *  [NOTE: In general, the _Thread_Dispatch_disable_level will
 *  only be 0 or 1 unless you are in an interrupt handler and that
 *  interrupt handler invokes the executive.]  When not inlined
 *  something calls _Thread_Enable_dispatch which in turns calls
 *  _Thread_Dispatch.  If the enable dispatch is inlined, then
 *  one subroutine call is avoided entirely.]
 */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE

/*
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  If TRUE, then the loops are unrolled.
 *  If FALSE, then the loops are not unrolled.
 *
 *  The primary factor in making this decision is the cost of disabling
 *  and enabling interrupts (_ISR_Flash) versus the cost of rest of the
 *  body of the loop.  On some CPUs, the flash is more expensive than
 *  one iteration of the loop body.  In this case, it might be desirable
 *  to unroll the loop.  It is important to note that on some CPUs, this
 *  code is the longest interrupt disable period in RTEMS.  So it is
 *  necessary to strike a balance when setting this parameter.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

/*
 *  Does this port provide a CPU dependent IDLE task implementation?
 *
 *  If TRUE, then the routine _CPU_Thread_Idle_body
 *  must be provided and is the default IDLE thread body instead of
 *  _CPU_Thread_Idle_body.
 *
 *  If FALSE, then use the generic IDLE thread body if the BSP does
 *  not provide one.
 *
 *  This is intended to allow for supporting processors which have
 *  a low power or idle mode.  When the IDLE thread is executed, then
 *  the CPU can be powered down.
 *
 *  The order of precedence for selecting the IDLE thread body is:
 *
 *    1.  BSP provided
 *    2.  CPU dependent (if provided)
 *    3.  generic (if no BSP and no CPU dependent)
 */

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 */

#define CPU_STACK_GROWS_UP               FALSE

/*
 *  The following is the variable attribute used to force alignment
 *  of critical RTEMS structures.  On some processors it may make
 *  sense to have these aligned on tighter boundaries than
 *  the minimum requirements of the compiler in order to have as
 *  much of the critical data area as possible in a cache line.
 *
 *  The placement of this macro in the declaration of the variables
 *  is based on the syntactically requirements of the GNU C
 *  "__attribute__" extension.  For example with GNU C, use
 *  the following to force a structures to a 32 byte boundary.
 *
 *      __attribute__ ((aligned (32)))
 *
 *  NOTE:  Currently only the Priority Bit Map table uses this feature.
 *         To benefit from using this, the data must be heavily
 *         used so it will stay in the cache and used frequently enough
 *         in the executive to justify turning this on.
 */

#define CPU_STRUCTURE_ALIGNMENT \
  __attribute__ ((aligned (PPC_STRUCTURE_ALIGNMENT)))

#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE
#else
#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE
#endif

/*
 *  Does the CPU have hardware floating point?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is supported.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is ignored.
 *
 *  If there is a FP coprocessor such as the i387 or mc68881, then
 *  the answer is TRUE.
 *
 *  The macro name "PPC_HAS_FPU" should be made CPU specific.
 *  It indicates whether or not this CPU model has FP support.  For
 *  example, it would be possible to have an i386_nofp CPU model
 *  which set this to false to indicate that you have an i386 without
 *  an i387 and wish to leave floating point support out of RTEMS.
 */

#if ( PPC_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#define CPU_SOFTWARE_FP     FALSE
#else
#define CPU_HARDWARE_FP     FALSE
#define CPU_SOFTWARE_FP     FALSE
#endif

/*
 *  Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
 *
 *  If CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.
 *
 *  PowerPC Note: It appears the GCC can implicitly generate FPU
 *  and Altivec instructions when you least expect them.  So make
 *  all tasks floating point.
 */

#define CPU_ALL_TASKS_ARE_FP CPU_HARDWARE_FP

/*
 *  Should the IDLE task have a floating point context?
 *
 *  If TRUE, then the IDLE task is created as a RTEMS_FLOATING_POINT task
 *  and it has a floating point context which is switched in and out.
 *  If FALSE, then the IDLE task does not have a floating point context.
 *
 *  Setting this to TRUE negatively impacts the time required to preempt
 *  the IDLE task from an interrupt because the floating point context
 *  must be saved as part of the preemption.
 */

#define CPU_IDLE_TASK_IS_FP      FALSE

/*
 *  Processor defined structures required for cpukit/score.
 */

/*
 * Contexts
 *
 *  Generally there are 2 types of context to save.
 *     1. Interrupt registers to save
 *     2. Task level registers to save
 *
 *  This means we have the following 3 context items:
 *     1. task level context stuff::  Context_Control
 *     2. floating point task stuff:: Context_Control_fp
 *     3. special interrupt level context :: Context_Control_interrupt
 *
 *  On some processors, it is cost-effective to save only the callee
 *  preserved registers during a task context switch.  This means
 *  that the ISR code needs to save those registers which do not
 *  persist across function calls.  It is not mandatory to make this
 *  distinctions between the caller/callee saves registers for the
 *  purpose of minimizing context saved during task switch and on interrupts.
 *  If the cost of saving extra registers is minimal, simplicity is the
 *  choice.  Save the same context on interrupt entry as for tasks in
 *  this case.
 *
 *  Additionally, if gdb is to be made aware of RTEMS tasks for this CPU, then
 *  care should be used in designing the context area.
 *
 *  On some CPUs with hardware floating point support, the Context_Control_fp
 *  structure will not be used or it simply consist of an array of a
 *  fixed number of bytes.   This is done when the floating point context
 *  is dumped by a "FP save context" type instruction and the format
 *  is not really defined by the CPU.  In this case, there is no need
 *  to figure out the exact format -- only the size.  Of course, although
 *  this is enough information for RTEMS, it is probably not enough for
 *  a debugger such as gdb.  But that is another problem.
 */

#ifndef __SPE__
  #define PPC_GPR_TYPE uint32_t
  #define PPC_GPR_SIZE 4
  #define PPC_GPR_LOAD lwz
  #define PPC_GPR_STORE stw
#else
  #define PPC_GPR_TYPE uint64_t
  #define PPC_GPR_SIZE 8
  #define PPC_GPR_LOAD evldd
  #define PPC_GPR_STORE evstdd
#endif

#define PPC_DEFAULT_CACHE_LINE_SIZE 32

#ifndef ASM

/* Non-volatile context according to E500ABIUG and EABI */
typedef struct {
  uint32_t gpr1;
  uint32_t msr;
  uint32_t lr;
  uint32_t cr;
  PPC_GPR_TYPE gpr14;
  PPC_GPR_TYPE gpr15;
  PPC_GPR_TYPE gpr16;
  PPC_GPR_TYPE gpr17;
  PPC_GPR_TYPE gpr18;
  PPC_GPR_TYPE gpr19;
  PPC_GPR_TYPE gpr20;
  PPC_GPR_TYPE gpr21;
  PPC_GPR_TYPE gpr22;
  PPC_GPR_TYPE gpr23;
  PPC_GPR_TYPE gpr24;
  PPC_GPR_TYPE gpr25;
  PPC_GPR_TYPE gpr26;
  PPC_GPR_TYPE gpr27;
  PPC_GPR_TYPE gpr28;
  PPC_GPR_TYPE gpr29;
  PPC_GPR_TYPE gpr30;
  PPC_GPR_TYPE gpr31;
  #ifdef __ALTIVEC__
    /*
     * 12 non-volatile vector registers, cache-aligned area for vscr/vrsave
     * and padding to ensure cache-alignment.  Unfortunately, we can't verify
     * the cache line size here in the cpukit but altivec support code will
     * produce an error if this is ever different from 32 bytes.
     * 
     * Note: it is the BSP/CPU-support's responsibility to save/restore
     *       volatile vregs across interrupts and exceptions.
     */
    uint8_t altivec[16*12 + 32 + PPC_DEFAULT_CACHE_LINE_SIZE];
  #endif
} ppc_context;

typedef struct {
  uint8_t context [
    PPC_DEFAULT_CACHE_LINE_SIZE
      + sizeof(ppc_context)
      + (sizeof(ppc_context) % PPC_DEFAULT_CACHE_LINE_SIZE == 0
        ? 0
          : PPC_DEFAULT_CACHE_LINE_SIZE
            - sizeof(ppc_context) % PPC_DEFAULT_CACHE_LINE_SIZE)
  ];
} Context_Control;

static inline ppc_context *ppc_get_context( Context_Control *context )
{
  uintptr_t clsz = PPC_DEFAULT_CACHE_LINE_SIZE;
  uintptr_t mask = clsz - 1;
  uintptr_t addr = (uintptr_t) context;

  return (ppc_context *) ((addr & ~mask) + clsz);
}

#define _CPU_Context_Get_SP( _context ) \
  ppc_get_context(_context)->gpr1
#endif /* ASM */

#define PPC_CONTEXT_OFFSET_GPR1 32
#define PPC_CONTEXT_OFFSET_MSR 36
#define PPC_CONTEXT_OFFSET_LR 40
#define PPC_CONTEXT_OFFSET_CR 44

#define PPC_CONTEXT_GPR_OFFSET( gpr ) \
  (((gpr) - 14) * PPC_GPR_SIZE + 48)

#define PPC_CONTEXT_OFFSET_GPR14 PPC_CONTEXT_GPR_OFFSET( 14 )
#define PPC_CONTEXT_OFFSET_GPR15 PPC_CONTEXT_GPR_OFFSET( 15 )
#define PPC_CONTEXT_OFFSET_GPR16 PPC_CONTEXT_GPR_OFFSET( 16 )
#define PPC_CONTEXT_OFFSET_GPR17 PPC_CONTEXT_GPR_OFFSET( 17 )
#define PPC_CONTEXT_OFFSET_GPR18 PPC_CONTEXT_GPR_OFFSET( 18 )
#define PPC_CONTEXT_OFFSET_GPR19 PPC_CONTEXT_GPR_OFFSET( 19 )
#define PPC_CONTEXT_OFFSET_GPR20 PPC_CONTEXT_GPR_OFFSET( 20 )
#define PPC_CONTEXT_OFFSET_GPR21 PPC_CONTEXT_GPR_OFFSET( 21 )
#define PPC_CONTEXT_OFFSET_GPR22 PPC_CONTEXT_GPR_OFFSET( 22 )
#define PPC_CONTEXT_OFFSET_GPR23 PPC_CONTEXT_GPR_OFFSET( 23 )
#define PPC_CONTEXT_OFFSET_GPR24 PPC_CONTEXT_GPR_OFFSET( 24 )
#define PPC_CONTEXT_OFFSET_GPR25 PPC_CONTEXT_GPR_OFFSET( 25 )
#define PPC_CONTEXT_OFFSET_GPR26 PPC_CONTEXT_GPR_OFFSET( 26 )
#define PPC_CONTEXT_OFFSET_GPR27 PPC_CONTEXT_GPR_OFFSET( 27 )
#define PPC_CONTEXT_OFFSET_GPR28 PPC_CONTEXT_GPR_OFFSET( 28 )
#define PPC_CONTEXT_OFFSET_GPR29 PPC_CONTEXT_GPR_OFFSET( 29 )
#define PPC_CONTEXT_OFFSET_GPR30 PPC_CONTEXT_GPR_OFFSET( 30 )
#define PPC_CONTEXT_OFFSET_GPR31 PPC_CONTEXT_GPR_OFFSET( 31 )

#ifndef ASM
typedef struct {
    /* The ABIs (PowerOpen/SVR4/EABI) only require saving f14-f31 over
     * procedure calls.  However, this would mean that the interrupt
     * frame had to hold f0-f13, and the fpscr.  And as the majority
     * of tasks will not have an FP context, we will save the whole
     * context here.
     */
#if (PPC_HAS_DOUBLE == 1)
    double	f[32];
    uint64_t	fpscr;
#else
    float	f[32];
    uint32_t	fpscr;
#endif
} Context_Control_fp;

typedef struct CPU_Interrupt_frame {
    uint32_t   stacklink;	/* Ensure this is a real frame (also reg1 save) */
    uint32_t   calleeLr;	/* link register used by callees: SVR4/EABI */

    /* This is what is left out of the primary contexts */
    uint32_t   gpr0;
    uint32_t   gpr2;		/* play safe */
    uint32_t   gpr3;
    uint32_t   gpr4;
    uint32_t   gpr5;
    uint32_t   gpr6;
    uint32_t   gpr7;
    uint32_t   gpr8;
    uint32_t   gpr9;
    uint32_t   gpr10;
    uint32_t   gpr11;
    uint32_t   gpr12;
    uint32_t   gpr13;   /* Play safe */
    uint32_t   gpr28;   /* For internal use by the IRQ handler */
    uint32_t   gpr29;   /* For internal use by the IRQ handler */
    uint32_t   gpr30;   /* For internal use by the IRQ handler */
    uint32_t   gpr31;   /* For internal use by the IRQ handler */
    uint32_t   cr;	/* Bits of this are volatile, so no-one may save */
    uint32_t   ctr;
    uint32_t   xer;
    uint32_t   lr;
    uint32_t   pc;
    uint32_t   msr;
    uint32_t   pad[3];
} CPU_Interrupt_frame;

#endif /* ASM */

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  PowerPC Specific Information:
 *
 *  The PowerPC and x86 were the first to use the PIC interrupt model.
 *  They do not use the simple vectored interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

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

#endif /* ndef ASM */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000001 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000001 /* interrupt level in mode */

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 */

#ifndef ASM

SCORE_EXTERN struct {
  uint32_t      *Disable_level;
  void          *Stack;
  volatile bool *Switch_necessary;
  bool          *Signal;

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
 *
 *  NOTE: CPU_INTERRUPT_NUMBER_OF_VECTORS and 
 *        CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER are only used on
 *        Simple Vectored Architectures and thus are not defined
 *        for this architecture.
 */

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
    msr |= ppc_interrupt_get_disable_mask();
  }
  else {
    msr &= ~ppc_interrupt_get_disable_mask();
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

#endif /* ASM */

#define _CPU_Fatal_halt( _error ) \
  _BSP_Fatal_error(_error)

/* end of Fatal Error manager macros */

/*
 * SPRG0 was previously used to make sure that the BSP fixed the PR288 bug.
 * Now SPRG0 is devoted to the interrupt disable mask.
 */

#define PPC_BSP_HAS_FIXED_PR288	ppc_this_is_now_the_interrupt_disable_mask

/*
 *  Should be large enough to run all RTEMS tests.  This ensures
 *  that a "reasonable" small application should not have any problems.
 */

#define CPU_STACK_MINIMUM_SIZE          (1024*8)

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 */

#define CPU_ALIGNMENT              (PPC_ALIGNMENT)

/*
 *  This number corresponds to the byte alignment requirement for the
 *  heap handler.  This alignment requirement may be stricter than that
 *  for the data types alignment specified by CPU_ALIGNMENT.  It is
 *  common for the heap to follow the same alignment requirement as
 *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
 *  then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_HEAP_ALIGNMENT         (PPC_ALIGNMENT)

/*
 *  This number corresponds to the byte alignment requirement for memory
 *  buffers allocated by the partition manager.  This alignment requirement
 *  may be stricter than that for the data types alignment specified by
 *  CPU_ALIGNMENT.  It is common for the partition to follow the same
 *  alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict
 *  enough for the partition, then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_PARTITION_ALIGNMENT    (PPC_ALIGNMENT)

/*
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 */

#define CPU_STACK_ALIGNMENT        (PPC_STACK_ALIGNMENT)

#ifndef ASM
/*  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version will work on any processor, but if there is a better
 *  way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *     swap least significant two bytes with 16-bit rotate
 *     swap upper and lower 16-bits
 *     swap most significant two bytes with 16-bit rotate
 *
 *  Some CPUs have special instructions which swap a 32-bit quantity in
 *  a single instruction (e.g. i486).  It is probably best to avoid
 *  an "endian swapping control bit" in the CPU.  One good reason is
 *  that interrupts would probably have to be disabled to ensure that
 *  an interrupt does not try to access the same "chunk" with the wrong
 *  endian.  Another good reason is that on some CPUs, the endian bit
 *  endianness for ALL fetches -- both code and data -- so the code
 *  will be fetched incorrectly.
 */

static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t   swapped;

  __asm__ volatile("rlwimi %0,%1,8,24,31;"
	       "rlwimi %0,%1,24,16,23;"
	       "rlwimi %0,%1,8,8,15;"
	       "rlwimi %0,%1,24,0,7;" :
	       "=&r" ((swapped)) : "r" ((value)));

  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */


#ifndef ASM
/* Context handler macros */

/*
 *  Initialize the context to a state suitable for starting a
 *  task after a context restore operation.  Generally, this
 *  involves:
 *
 *     - setting a starting address
 *     - preparing the stack
 *     - preparing the stack and frame pointers
 *     - setting the proper interrupt level in the context
 *     - initializing the floating point context
 *
 *  This routine generally does not set any unnecessary register
 *  in the context.  The state of the "general data" registers is
 *  undefined at task start time.
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
);

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/*
 *  The purpose of this macro is to allow the initial pointer into
 *  a floating point context area (used to save the floating point
 *  context) to be at an arbitrary place in the floating point
 *  context area.
 *
 *  This is necessary because some FP units are designed to have
 *  their context saved as a stack which grows into lower addresses.
 *  Other FP units can be saved by simply moving registers into offsets
 *  from the base of the context area.  Finally some FP units provide
 *  a "dump context" instruction which could fill in from high to low
 *  or low to high based on the whim of the CPU designers.
 */

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

/*
 *  This routine initializes the FP context area passed to it to.
 *  There are a few standard ways in which to initialize the
 *  floating point context.  The code included for this macro assumes
 *  that this is a CPU in which a "initial" FP context was saved into
 *  _CPU_Null_fp_context and it simply copies it to the destination
 *  context passed to it.
 *
 *  Other models include (1) not doing anything, and (2) putting
 *  a "null FP status word" in the correct place in the FP context.
 */

#define _CPU_Context_Initialize_fp( _destination ) \
  memset( *(_destination), 0, sizeof( **(_destination) ) )

/* end of Context handler macros */
#endif /* ASM */

#ifndef ASM
/* Bitfield handler macros */

/*
 *  This routine sets _output to the bit number of the first bit
 *  set in _value.  _value is of CPU dependent type Priority_bit_map_Control.
 *  This type may be either 16 or 32 bits wide although only the 16
 *  least significant bits will be used.
 *
 *  There are a number of variables in using a "find first bit" type
 *  instruction.
 *
 *    (1) What happens when run on a value of zero?
 *    (2) Bits may be numbered from MSB to LSB or vice-versa.
 *    (3) The numbering may be zero or one based.
 *    (4) The "find first bit" instruction may search from MSB or LSB.
 *
 *  RTEMS guarantees that (1) will never happen so it is not a concern.
 *  (2),(3), (4) are handled by the macros _CPU_Priority_mask() and
 *  _CPU_Priority_Bits_index().  These three form a set of routines
 *  which must logically operate together.  Bits in the _value are
 *  set and cleared based on masks built by _CPU_Priority_mask().
 *  The basic major and minor values calculated by _Priority_Major()
 *  and _Priority_Minor() are "massaged" by _CPU_Priority_Bits_index()
 *  to properly range between the values returned by the "find first bit"
 *  instruction.  This makes it possible for _Priority_Get_highest() to
 *  calculate the major and directly index into the minor table.
 *  This mapping is necessary to ensure that 0 (a high priority major/minor)
 *  is the first bit found.
 *
 *  This entire "find first bit" and mapping process depends heavily
 *  on the manner in which a priority is broken into a major and minor
 *  components with the major being the 4 MSB of a priority and minor
 *  the 4 LSB.  Thus (0 << 4) + 0 corresponds to priority 0 -- the highest
 *  priority.  And (15 << 4) + 14 corresponds to priority 254 -- the next
 *  to the lowest priority.
 *
 *  If your CPU does not have a "find first bit" instruction, then
 *  there are ways to make do without it.  Here are a handful of ways
 *  to implement this in software:
 *
 *    - a series of 16 bit test instructions
 *    - a "binary search using if's"
 *    - _number = 0
 *      if _value > 0x00ff
 *        _value >>=8
 *        _number = 8;
 *
 *      if _value > 0x0000f
 *        _value >=8
 *        _number += 4
 *
 *      _number += bit_set_table[ _value ]
 *
 *    where bit_set_table[ 16 ] has values which indicate the first
 *      bit set
 */

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    __asm__ volatile ("cntlzw %0, %1" : "=r" ((_output)), "=r" ((_value)) : \
		  "1" ((_value))); \
  }

/* end of Bitfield handler macros */

/*
 *  This routine builds the mask which corresponds to the bit fields
 *  as searched by _CPU_Bitfield_Find_first_bit().  See the discussion
 *  for that routine.
 */

#define _CPU_Priority_Mask( _bit_number ) \
  ( 0x80000000 >> (_bit_number) )

/*
 *  This routine translates the bit numbers returned by
 *  _CPU_Bitfield_Find_first_bit() into something suitable for use as
 *  a major or minor component of a priority.  See the discussion
 *  for that routine.
 */

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

/* end of Priority handler macros */
#endif /* ASM */

/* functions */

#ifndef ASM

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(void);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_Install_interrupt_stack
 *
 *  This routine installs the hardware interrupt stack pointer.
 *
 *  NOTE:  It need only be provided if CPU_HAS_HARDWARE_INTERRUPT_STACK
 *         is TRUE.
 */

void _CPU_Install_interrupt_stack( void );

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/*
 *  _CPU_Context_restore
 *
 *  This routine is generallu used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 */

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

/*
 * _CPU_Initialize_altivec()
 *
 * Global altivec-related initialization.
 */
void
_CPU_Initialize_altivec(void);

/*
 * _CPU_Context_switch_altivec
 *
 * This routine switches the altivec contexts passed to it.
 */

void
_CPU_Context_switch_altivec(
  Context_Control *from,
  Context_Control *to
);

/*
 * _CPU_Context_restore_altivec
 *
 * This routine restores the altivec context passed to it.
 */

void
_CPU_Context_restore_altivec(
  Context_Control *ctxt
);

/*
 * _CPU_Context_initialize_altivec
 *
 * This routine initializes the altivec context passed to it.
 */

void
_CPU_Context_initialize_altivec(
  Context_Control *ctxt
);

void _CPU_Fatal_error(
  uint32_t   _error
);

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_CPU_H */
