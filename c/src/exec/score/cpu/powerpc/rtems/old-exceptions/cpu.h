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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/ppc.h>               /* pick up machine definitions */
#ifndef ASM
struct CPU_Interrupt_frame;
typedef void ( *ppc_isr_entry )( int, struct CPU_Interrupt_frame * );

#include <rtems/score/ppctypes.h>
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
 *  This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE
 *  or CPU_INSTALL_HARDWARE_INTERRUPT_STACK is TRUE.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 1

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
 *  So far, the only CPU in which this option has been used is the
 *  HP PA-RISC.  The HP C compiler and gcc both implicitly use the
 *  floating point registers to perform integer multiplies.  If
 *  a function which you would not think utilize the FP unit DOES,
 *  then one can not easily predict which tasks will use the FP hardware.
 *  In this case, this option should be TRUE.
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
 */
/*
 *  ACB Note:  This could make debugging tricky..
 */

#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

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
 *  Processor defined structures
 *
 *  Examples structures include the descriptor tables from the i386
 *  and the processor control structure on the i960ca.
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

typedef struct {
    unsigned32 gpr1;	/* Stack pointer for all */
    unsigned32 gpr2;	/* TOC in PowerOpen, reserved SVR4, section ptr EABI + */
    unsigned32 gpr13;	/* First non volatile PowerOpen, section ptr SVR4/EABI */
    unsigned32 gpr14;	/* Non volatile for all */
    unsigned32 gpr15;	/* Non volatile for all */
    unsigned32 gpr16;	/* Non volatile for all */
    unsigned32 gpr17;	/* Non volatile for all */
    unsigned32 gpr18;	/* Non volatile for all */
    unsigned32 gpr19;	/* Non volatile for all */
    unsigned32 gpr20;	/* Non volatile for all */
    unsigned32 gpr21;	/* Non volatile for all */
    unsigned32 gpr22;	/* Non volatile for all */
    unsigned32 gpr23;	/* Non volatile for all */
    unsigned32 gpr24;	/* Non volatile for all */
    unsigned32 gpr25;	/* Non volatile for all */
    unsigned32 gpr26;	/* Non volatile for all */
    unsigned32 gpr27;	/* Non volatile for all */
    unsigned32 gpr28;	/* Non volatile for all */
    unsigned32 gpr29;	/* Non volatile for all */
    unsigned32 gpr30;	/* Non volatile for all */
    unsigned32 gpr31;	/* Non volatile for all */
    unsigned32 cr;	/* PART of the CR is non volatile for all */
    unsigned32 pc;	/* Program counter/Link register */
    unsigned32 msr;	/* Initial interrupt level */
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
    unsigned32 stacklink;	/* Ensure this is a real frame (also reg1 save) */
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
    unsigned32 dummy[13];	/* Used by callees: PowerOpen ABI */
#else
    unsigned32 dummy[1];	/* Used by callees: SVR4/EABI */
#endif
    /* This is what is left out of the primary contexts */
    unsigned32 gpr0;
    unsigned32 gpr2;		/* play safe */
    unsigned32 gpr3;
    unsigned32 gpr4;
    unsigned32 gpr5;
    unsigned32 gpr6;
    unsigned32 gpr7;
    unsigned32 gpr8;
    unsigned32 gpr9;
    unsigned32 gpr10;
    unsigned32 gpr11;
    unsigned32 gpr12;
    unsigned32 gpr13;   /* Play safe */
    unsigned32 gpr28;   /* For internal use by the IRQ handler */
    unsigned32 gpr29;   /* For internal use by the IRQ handler */
    unsigned32 gpr30;   /* For internal use by the IRQ handler */
    unsigned32 gpr31;   /* For internal use by the IRQ handler */
    unsigned32 cr;	/* Bits of this are volatile, so no-one may save */
    unsigned32 ctr;
    unsigned32 xer;
    unsigned32 lr;
    unsigned32 pc;
    unsigned32 msr;
    unsigned32 pad[3];
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
  unsigned32   idle_task_stack_size;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */

  unsigned32   clicks_per_usec;	       /* Timer clicks per microsecond */
  void       (*spurious_handler)(unsigned32 vector, CPU_Interrupt_frame *);
  boolean      exceptions_in_RAM;     /* TRUE if in RAM */

#if (defined(ppc403) || defined(ppc405) || defined(mpc860) || defined(mpc821))
  unsigned32   serial_per_sec;	       /* Serial clocks per second */
  boolean      serial_external_clock;
  boolean      serial_xon_xoff;
  boolean      serial_cts_rts;
  unsigned32   serial_rate;
  unsigned32   timer_average_overhead; /* Average overhead of timer in ticks */
  unsigned32   timer_least_valid;      /* Least valid number from timer      */
  boolean      timer_internal_clock;   /* TRUE, when timer runs with CPU clk */
#endif

#if (defined(mpc860) || defined(mpc821))
  unsigned32   clock_speed;            /* Speed of CPU in Hz */
#endif
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 */

/*
 *  Macros to access PowerPC specific additions to the CPU Table
 */

#define rtems_cpu_configuration_get_clicks_per_usec() \
   (_CPU_Table.clicks_per_usec)

#define rtems_cpu_configuration_get_spurious_handler() \
   (_CPU_Table.spurious_handler)

#define rtems_cpu_configuration_get_exceptions_in_ram() \
   (_CPU_Table.exceptions_in_RAM)

#if (defined(ppc403) || defined(ppc405) || defined(mpc860) || defined(mpc821))

#define rtems_cpu_configuration_get_serial_per_sec() \
   (_CPU_Table.serial_per_sec)

#define rtems_cpu_configuration_get_serial_external_clock() \
   (_CPU_Table.serial_external_clock)

#define rtems_cpu_configuration_get_serial_xon_xoff() \
   (_CPU_Table.serial_xon_xoff)

#define rtems_cpu_configuration_get_serial_cts_rts() \
   (_CPU_Table.serial_cts_rts)

#define rtems_cpu_configuration_get_serial_rate() \
   (_CPU_Table.serial_rate)

#define rtems_cpu_configuration_get_timer_average_overhead() \
   (_CPU_Table.timer_average_overhead)

#define rtems_cpu_configuration_get_timer_least_valid() \
   (_CPU_Table.timer_least_valid)

#define rtems_cpu_configuration_get_timer_internal_clock() \
   (_CPU_Table.timer_internal_clock)

#endif

#if (defined(mpc860) || defined(mpc821))
#define rtems_cpu_configuration_get_clock_speed() \
   (_CPU_Table.clock_speed)
#endif


/*
 *  The following type defines an entry in the PPC's trap table.
 *
 *  NOTE: The instructions chosen are RTEMS dependent although one is
 *        obligated to use two of the four instructions to perform a
 *        long jump.  The other instructions load one register with the
 *        trap type (a.k.a. vector) and another with the psr.
 */
 
typedef struct {
  unsigned32   stwu_r1;                       /* stwu  %r1, -(??+IP_END)(%1)*/
  unsigned32   stw_r0;                        /* stw   %r0, IP_0(%r1)       */
  unsigned32   li_r0_IRQ;                     /* li    %r0, _IRQ            */
  unsigned32   b_Handler;                     /* b     PROC (_ISR_Handler)  */
} CPU_Trap_table_entry;

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


SCORE_EXTERN struct {
  unsigned32 volatile* Nest_level;
  unsigned32 volatile* Disable_level;
  void *Vector_table;
  void *Stack;
#if (PPC_ABI == PPC_ABI_POWEROPEN)
  unsigned32 Dispatch_r2;
#else
  unsigned32 Default_r2;
#if (PPC_ABI != PPC_ABI_GCC27)
  unsigned32 Default_r13;
#endif
#endif
  volatile boolean *Switch_necessary;
  boolean *Signal;

  unsigned32 msr_initial;
} _CPU_IRQ_info CPU_STRUCTURE_ALIGNMENT;

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

void _CPU_Initialize_vectors(void);

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _isr_cookie.
 */

#define loc_string(a,b)	a " (" #b ")\n"

#define _CPU_MSR_Value( _msr_value ) \
  do { \
    _msr_value = 0; \
    asm volatile ("mfmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

#define _CPU_MSR_SET( _msr_value ) \
{ asm volatile ("mtmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

#if 0
#define _CPU_ISR_Disable( _isr_cookie ) \
  { register unsigned int _disable_mask = PPC_MSR_DISABLE_MASK; \
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
  { register unsigned int _disable_mask = PPC_MSR_DISABLE_MASK; \
    _isr_cookie = 0; \
    asm volatile ( \
	"mfmsr %0; andc %1,%0,%1; mtmsr %1" : \
	"=&r" ((_isr_cookie)), "=&r" ((_disable_mask)) : \
	"0" ((_isr_cookie)), "1" ((_disable_mask)) \
	); \
  }


#define _CPU_Data_Cache_Block_Flush( _address ) \
  do { register void *__address = (_address); \
       register unsigned32 _zero = 0; \
       asm volatile ( "dcbf %0,%1" : \
		      "=r" (_zero), "=r" (__address) : \
                      "0" (_zero), "1" (__address) \
       ); \
  } while (0)

#define _CPU_Data_Cache_Block_Invalidate( _address ) \
  do { register void *__address = (_address); \
       register unsigned32 _zero = 0; \
       asm volatile ( "dcbi %0,%1" : \
		      "=r" (_zero), "=r" (__address) : \
                      "0" (_zero), "1" (__address) \
       ); \
  } while (0)


/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _isr_cookie is not modified.
 */

#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
     asm volatile ( "mtmsr %0" : \
		   "=r" ((_isr_cookie)) : \
                   "0" ((_isr_cookie))); \
  }

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

#define _CPU_ISR_Flash( _isr_cookie ) \
  { register unsigned int _disable_mask = PPC_MSR_DISABLE_MASK; \
    asm volatile ( \
      "mtmsr %0; andc %1,%0,%1; mtmsr %1" : \
      "=r" ((_isr_cookie)), "=r" ((_disable_mask)) : \
      "0" ((_isr_cookie)), "1" ((_disable_mask)) \
    ); \
  }

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

unsigned32 _CPU_ISR_Calculate_level(
  unsigned32 new_level
);

void _CPU_ISR_Set_level(
  unsigned32 new_level
);
  
unsigned32 _CPU_ISR_Get_level( void );

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/* end of ISR handler macros */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define CPU_Get_timebase_low( _value ) \
    asm volatile( "mftb  %0" : "=r" (_value) )

#define rtems_bsp_delay( _microseconds ) \
  do { \
    unsigned32 start, ticks, now; \
    CPU_Get_timebase_low( start ) ; \
    ticks = (_microseconds) * _CPU_Table.clicks_per_usec; \
    do \
      CPU_Get_timebase_low( now ) ; \
    while (now - start < ticks); \
  } while (0)

#define rtems_bsp_delay_in_bus_cycles( _cycles ) \
  do { \
    unsigned32 start, now; \
    CPU_Get_timebase_low( start ); \
    do \
      CPU_Get_timebase_low( now ); \
    while (now - start < (_cycles)); \
  } while (0)



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
 *
 *  NOTE:  Implemented as a subroutine for the SPARC port.
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  unsigned32       *stack_base,
  unsigned32        size,
  unsigned32        new_level,
  void             *entry_point,
  boolean           is_fp
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
  { \
   ((Context_Control_fp *) *((void **) _destination))->fpscr = PPC_INIT_FPSCR; \
  }

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 */

#define _CPU_Fatal_halt( _error ) \
  _CPU_Fatal_error(_error)

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

/*
 *  This routine sets _output to the bit number of the first bit
 *  set in _value.  _value is of CPU dependent type Priority_Bit_map_control.
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
    asm volatile ("cntlzw %0, %1" : "=r" ((_output)), "=r" ((_value)) : \
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

/* variables */

extern const unsigned32 _CPU_msrs[4];

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void            (*thread_dispatch)
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
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
);

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
);

void _CPU_Fatal_error(
  unsigned32 _error
);

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
 *  that interrupts would probably have to be disabled to insure that
 *  an interrupt does not try to access the same "chunk" with the wrong
 *  endian.  Another good reason is that on some CPUs, the endian bit
 *  endianness for ALL fetches -- both code and data -- so the code
 *  will be fetched incorrectly.
 */
 
static inline unsigned int CPU_swap_u32(
  unsigned int value
)
{
  unsigned32 swapped;
 
  asm volatile("rlwimi %0,%1,8,24,31;"
	       "rlwimi %0,%1,24,16,23;"
	       "rlwimi %0,%1,8,8,15;"
	       "rlwimi %0,%1,24,0,7;" :
	       "=&r" ((swapped)) : "r" ((value)));

  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

/*
 *  Routines to access the decrementer register
 */

#define PPC_Set_decrementer( _clicks ) \
  do { \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

/*
 *  Routines to access the time base register
 */

static inline unsigned64 PPC_Get_timebase_register( void )
{
  unsigned32 tbr_low;
  unsigned32 tbr_high;
  unsigned32 tbr_high_old;
  unsigned64 tbr;

  do {
    asm volatile( "mftbu %0" : "=r" (tbr_high_old));
    asm volatile( "mftb  %0" : "=r" (tbr_low));
    asm volatile( "mftbu %0" : "=r" (tbr_high));
  } while ( tbr_high_old != tbr_high );

  tbr = tbr_high;
  tbr <<= 32;
  tbr |= tbr_low;
  return tbr;
}

#ifdef __cplusplus
}
#endif

#endif
