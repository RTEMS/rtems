/*  cpu.h
 *
 *  This include file contains macros pertaining to the Opencores
 *  or1k processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  This file adapted from no_cpu example of the RTEMS distribution.
 *  The body has been modified for the Opencores Or1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 *
 */

#ifndef _OR1K_CPU_h
#define _OR1K_CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include "rtems/score/or1k.h"            /* pick up machine definitions */
#ifndef ASM
#include "rtems/score/or1ktypes.h"
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
 *
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
 *
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

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
 *
 *  For the first cut of an Or1k implementation, let's not worry
 *  about this, and assume that our C code will autoperform any
 *  frame/stack allocation for us when the procedure is entered.
 *  If we write assembly code, we may have to deal with this manually.
 *  This can be changed later if we find it is impossible. This
 *  behavior is desireable as it allows us to work in low memory
 *  environments where we don't have room for a dedicated stack.
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
 *
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
 *
 */

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 *
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
 *  The macro name "OR1K_HAS_FPU" should be made CPU specific.
 *  It indicates whether or not this CPU model has FP support.  For
 *  example, it would be possible to have an i386_nofp CPU model
 *  which set this to false to indicate that you have an i386 without
 *  an i387 and wish to leave floating point support out of RTEMS.
 *
 *  The CPU_SOFTWARE_FP is used to indicate whether or not there
 *  is software implemented floating point that must be context 
 *  switched.  The determination of whether or not this applies
 *  is very tool specific and the state saved/restored is also
 *  compiler specific.
 *
 *  Or1k Specific Information:
 *
 *  At this time there are no implementations of Or1k that are
 *  expected to implement floating point. More importantly, the
 *  floating point architecture is expected to change significantly
 *  before such chips are fabricated.
 */

#if ( OR1K_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#define CPU_SOFTWARE_FP     FALSE
#else
#define CPU_HARDWARE_FP     FALSE
#define CPU_SOFTWARE_FP     TRUE
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
 *
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
 *
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
 *
 */

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 *
 *  Or1k Specific Information:
 *  
 *  Previously I had misread the documentation and set this
 *  to true. Surprisingly, it seemed to work anyway. I'm
 *  therefore not 100% sure exactly what this does. It should
 *  be correct as it is now, however. 
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
 *
 */

#define CPU_STRUCTURE_ALIGNMENT __attribute__ ((aligned (32)))

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 *
 *  Or1k Specific Information:
 *
 *  This version of RTEMS is designed specifically to run with
 *  big endian architectures. If you want little endian, you'll
 *  have to make the appropriate adjustments here and write
 *  efficient routines for byte swapping. The Or1k architecture
 *  doesn't do this very well.
 */

#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 */

#define CPU_MODES_INTERRUPT_MASK   0x00000001

/*
 *  Processor defined structures
 *
 *  Examples structures include the descriptor tables from the i386
 *  and the processor control structure on the i960ca.
 *
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
 *
 *  
 */

#ifdef OR1K_64BIT_ARCH
#define or1kreg unsigned64
#else
#define or1kreg unsigned32
#endif

/* SR_MASK is the mask of values that will be copied to/from the status
   register on a context switch. Some values, like the flag state, are
   specific on the context, while others, such as interrupt enables,
   are global. The currently defined global bits are:

   0x00001 SUPV:     Supervisor mode
   0x00002 EXR:      Exceptions on/off
   0x00004 EIR:      Interrupts enabled/disabled
   0x00008 DCE:      Data cache enabled/disabled
   0x00010 ICE:      Instruction cache enabled/disabled
   0x00020 DME:      Data MMU enabled/disabled
   0x00040 IME:      Instruction MMU enabled/disabled
   0x00080 LEE:      Little/Big Endian enable
   0x00100 CE:       Context ID/shadow regs enabled/disabled
   0x01000 OVE:      Overflow causes exception
   0x04000 EP:       Exceptions @ 0x0 or 0xF0000000
   0x08000 PXR:      Partial exception recognition enabled/disabled
   0x10000 SUMRA:    SPR's accessible/inaccessible

   The context specific bits are:

   0x00200 F         Branch flag indicator
   0x00400 CY        Carry flag indicator
   0x00800 OV        Overflow flag indicator
   0x02000 DSX       Delay slot exception occurred
   0xF8000000 CID    Current Context ID
*/

#define SR_MASK 0xF8002E00

typedef enum {
  SR_SUPV = 0x00001,
  SR_EXR = 0x00002,
  SR_EIR = 0x00004,
  SR_DCE = 0x00008,
  SR_ICE = 0x00010,
  SR_DME = 0x00020,
  SR_IME = 0x00040,
  SR_LEE = 0x00080,
  SR_CE = 0x00100,
  SR_F = 0x00200,
  SR_CY = 0x00400,
  SR_OV = 0x00800,
  SR_OVE = 0x01000,
  SR_DSX = 0x02000,
  SR_EP = 0x04000,
  SR_PXR = 0x08000,
  SR_SUMRA = 0x10000,
  SR_CID = 0xF8000000,
} StatusRegisterBits;

typedef struct {
  unsigned32  sr;     /* Current status register non persistent values */
  unsigned32  esr;    /* Saved exception status register */
  unsigned32  ear;    /* Saved exception effective address register */
  unsigned32  epc;    /* Saved exception PC register    */
  or1kreg     r[31];  /* Registers */
  or1kreg     pc;     /* Context PC 4 or 8 bytes for 64 bit alignment */
} Context_Control;

typedef int Context_Control_fp;
typedef Context_Control CPU_Interrupt_frame;
#define _CPU_Null_fp_context 0
#define _CPU_Interrupt_stack_low 0
#define _CPU_Interrupt_stack_high 0

/*
 *  The following table contains the information required to configure
 *  the XXX processor specific parameters.
 *
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
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 *
 */

/*
 *  Macros to access OR1K specific additions to the CPU Table
 *
 */

/* There are no CPU specific additions to the CPU Table for this port. */

/*
 *  This variable is optional.  It is used on CPUs on which it is difficult
 *  to generate an "uninitialized" FP context.  It is filled in by
 *  _CPU_Initialize and copied into the task's FP context area during
 *  _CPU_Context_Initialize.
 *
 */

/* SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context; */

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
 *
 */

/*
SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;
*/

/*
 *  With some compilation systems, it is difficult if not impossible to
 *  call a high-level language routine from assembly language.  This
 *  is especially true of commercial Ada compilers and name mangling
 *  C++ ones.  This variable can be optionally defined by the CPU porter
 *  and contains the address of the routine _Thread_Dispatch.  This
 *  can make it easier to invoke that routine at the end of the interrupt
 *  sequence (if a dispatch is necessary).
 *
 */

SCORE_EXTERN void           (*_CPU_Thread_dispatch_pointer)();

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 *
 */

/* XXX: if needed, put more variables here */

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 *
 *  Or1k Specific Information:
 *
 *  We don't support floating point in this version, so the size is 0
 */

#define CPU_CONTEXT_FP_SIZE 0

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 *
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by RTEMS.
 *
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      16
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  Should be large enough to run all RTEMS tests.  This insures
 *  that a "reasonable" small application should not have any problems.
 *
 */

#define CPU_STACK_MINIMUM_SIZE          4096

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 */

#define CPU_ALIGNMENT              8

/*
 *  This number corresponds to the byte alignment requirement for the
 *  heap handler.  This alignment requirement may be stricter than that
 *  for the data types alignment specified by CPU_ALIGNMENT.  It is
 *  common for the heap to follow the same alignment requirement as
 *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
 *  then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2 although it should be
 *         a multiple of 2 greater than or equal to 2.  The requirement
 *         to be a multiple of 2 is because the heap uses the least 
 *         significant field of the front and back flags to indicate
 *         that a block is in use or free.  So you do not want any odd
 *         length blocks really putting length data in that bit.
 *
 *         On byte oriented architectures, CPU_HEAP_ALIGNMENT normally will
 *         have to be greater or equal to than CPU_ALIGNMENT to ensure that
 *         elements allocated from the heap meet all restrictions.
 *
 */

#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

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
 *
 */

#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

/*
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 *
 */

#define CPU_STACK_ALIGNMENT        0

/* ISR handler macros */

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 *
 */

#define _CPU_ISR_Disable( _isr_cookie ) \
  { \
    (_isr_cookie) = 0;   /* do something to prevent warnings */ \
  }

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _level is not modified.
 *
 */

#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
  }

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 * modified.
 *
 */

#define _CPU_ISR_Flash( _isr_cookie ) \
  { \
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
 *
 *  The get routine usually must be implemented as a subroutine.
 *
 */

#define _CPU_ISR_Set_level( new_level ) \
  { \
  }

unsigned32 _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

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
 *  NOTE: This is_fp parameter is TRUE if the thread is to be a floating
 *        point thread.  This is typically only used on CPUs where the
 *        FPU may be easily disabled by software such as on the SPARC
 *        where the PSR contains an enable FPU bit.
 *
 */

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp ) \
  { \
  memset(_the_context,'\0',sizeof(Context_Control)); \
  (_the_context)->r[1] = (unsigned32*) ((unsigned32) (_stack_base) + (_size) ); \
  (_the_context)->r[2] = (unsigned32*) ((unsigned32) (_stack_base)); \
  (_the_context)->sr  = (_isr) ? 0x0000001B : 0x0000001F; \
  (_the_context)->pc  = (unsigned32*) _entry_point ; \
  }

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 *
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
 *
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
 *
 */

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   *((Context_Control_fp *) *((void **) _destination)) = _CPU_Null_fp_context; \
  }

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 *
 */

#define _CPU_Fatal_halt( _error ) \
  { \
  }

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
 *  _CPU_Priority_bits_index().  These three form a set of routines
 *  which must logically operate together.  Bits in the _value are
 *  set and cleared based on masks built by _CPU_Priority_mask().
 *  The basic major and minor values calculated by _Priority_Major()
 *  and _Priority_Minor() are "massaged" by _CPU_Priority_bits_index()
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
 *
 */

  /* #define CPU_USE_GENERIC_BITFIELD_CODE FALSE */
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE 
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

  /* Get a value between 0 and N where N is the bit size */
  /* This routine makes use of the fact that CPUCFGR defines
     OB32S to have value 32, and OB64S to have value 64. If
     this ever changes then this routine will fail. */
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
     asm volatile ("l.mfspr %0,r0,0x2   \n\t"\
                   "l.andi  %0,%0,0x60  \n\t"\
                   "l.ff1   %1,%1,r0    \n\t"\
                   "l.sub   %0,%0,%1    \n\t" : "=&r" (_output), "+r" (_value));

#endif
   
/* end of Bitfield handler macros */

/*
 *  This routine builds the mask which corresponds to the bit fields
 *  as searched by _CPU_Bitfield_Find_first_bit().  See the discussion
 *  for that routine.
 *
 */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
    (1 << _bit_number)

#endif

/*
 *  This routine translates the bit numbers returned by
 *  _CPU_Bitfield_Find_first_bit() into something suitable for use as
 *  a major or minor component of a priority.  See the discussion
 *  for that routine.
 *
 */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

#endif

/* end of Priority handler macros */

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 *
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)
);

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs a "raw" interrupt handler directly into the 
 *  processor's vector table.
 *
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
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
 *
 */

void _CPU_Install_interrupt_stack( void );

/*
 *  _CPU_Thread_Idle_body
 *
 *  This routine is the CPU dependent IDLE thread body.
 *
 *  NOTE:  It need only be provided if CPU_PROVIDES_IDLE_THREAD_BODY
 *         is TRUE.
 *
 */

void _CPU_Thread_Idle_body( void );

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 *
 *  Or1k Specific Information:
 *
 *  Please see the comments in the .c file for a description of how
 *  this function works. There are several things to be aware of.
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 */

void _CPU_Context_restore(
  Context_Control *new_context
);

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 *
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 *
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
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
 *
 */
 
static inline unsigned int CPU_swap_u32(
  unsigned int value
)
{
  unsigned32 byte1, byte2, byte3, byte4, swapped;
 
  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;
 
  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#ifdef __cplusplus
}
#endif

#endif
