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
 *  COPYRIGHT (c) 1989-1997.
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
 *  This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE
 *  or CPU_INSTALL_HARDWARE_INTERRUPT_STACK is TRUE.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

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
#else
#define CPU_HARDWARE_FP     FALSE
#endif

/*
 *  Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
 *
 *  If CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.
 */

#define CPU_ALL_TASKS_ARE_FP     FALSE

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
  __attribute__ ((aligned (PPC_CACHE_ALIGNMENT)))

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE


/*
 *  Processor defined structures required for cpukit/score.
 */

/* may need to put some structures here.  */

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

#ifndef ASM

typedef struct {
    uint32_t   gpr1;	/* Stack pointer for all */
    uint32_t   gpr2;	/* TOC in PowerOpen, reserved SVR4, section ptr EABI + */
    uint32_t   gpr13;	/* First non volatile PowerOpen, section ptr SVR4/EABI */
    uint32_t   gpr14;	/* Non volatile for all */
    uint32_t   gpr15;	/* Non volatile for all */
    uint32_t   gpr16;	/* Non volatile for all */
    uint32_t   gpr17;	/* Non volatile for all */
    uint32_t   gpr18;	/* Non volatile for all */
    uint32_t   gpr19;	/* Non volatile for all */
    uint32_t   gpr20;	/* Non volatile for all */
    uint32_t   gpr21;	/* Non volatile for all */
    uint32_t   gpr22;	/* Non volatile for all */
    uint32_t   gpr23;	/* Non volatile for all */
    uint32_t   gpr24;	/* Non volatile for all */
    uint32_t   gpr25;	/* Non volatile for all */
    uint32_t   gpr26;	/* Non volatile for all */
    uint32_t   gpr27;	/* Non volatile for all */
    uint32_t   gpr28;	/* Non volatile for all */
    uint32_t   gpr29;	/* Non volatile for all */
    uint32_t   gpr30;	/* Non volatile for all */
    uint32_t   gpr31;	/* Non volatile for all */
    uint32_t   cr;	/* PART of the CR is non volatile for all */
    uint32_t   pc;	/* Program counter/Link register */
    uint32_t   msr;	/* Initial interrupt level */
} Context_Control;

typedef struct {
    /* The ABIs (PowerOpen/SVR4/EABI) only require saving f14-f31 over
     * procedure calls.  However, this would mean that the interrupt
     * frame had to hold f0-f13, and the fpscr.  And as the majority
     * of tasks will not have an FP context, we will save the whole
     * context here.
     */
#if (PPC_HAS_DOUBLE == 1)
    double	f[32];
    double	fpscr;
#else
    float	f[32];
    float	fpscr;
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
  
/*
 *  The following table contains the information required to configure
 *  the PowerPC processor specific parameters.
 */

typedef struct {
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  uint32_t     idle_task_stack_size;
  uint32_t     interrupt_stack_size;
  uint32_t     extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( uint32_t   );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */

  uint32_t     clicks_per_usec;	       /* Timer clicks per microsecond */
  boolean      exceptions_in_RAM;     /* TRUE if in RAM */

#if (defined(ppc403) || defined(mpc860) || defined(mpc821) || defined(mpc8260))
  uint32_t     serial_per_sec;	       /* Serial clocks per second */
  boolean      serial_external_clock;
  boolean      serial_xon_xoff;
  boolean      serial_cts_rts;
  uint32_t     serial_rate;
  uint32_t     timer_average_overhead; /* Average overhead of timer in ticks */
  uint32_t     timer_least_valid;      /* Least valid number from timer      */
  boolean      timer_internal_clock;   /* TRUE, when timer runs with CPU clk */
#endif

#if (defined(mpc555) || defined(mpc860) || defined(mpc821) || defined(mpc8260))
  uint32_t     clock_speed;            /* Speed of CPU in Hz */
#endif
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 */

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

#define CPU_INTERRUPT_NUMBER_OF_VECTORS     (PPC_INTERRUPT_MAX)
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (PPC_INTERRUPT_MAX - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level. Note that
 *  this is not an option - RTEMS/score _relies_ on _ISR_Nest_level
 *  being maintained (e.g. watchdog queues).
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Should be large enough to run all RTEMS tests.  This insures
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
