/*  cpu.h
 *
 *  This include file contains information pertaining to the C4x
 *  processor.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/c4x.h>            /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/types.h>
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
 *  C4x Specific Information:
 *
 *  We might as well try to inline this code until there is a 
 *  code space problem.
 */

#define CPU_INLINE_ENABLE_DISPATCH       TRUE

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
 *  C4x Specific Information:
 *
 *  We might as well unroll this loop until there is a reason not to do so.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/*
 *  Does RTEMS manage a dedicated interrupt stack in software?
 *
 *  If TRUE, then a stack is allocated in _Interrupt_Manager_initialization.
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
 *  C4x Specific Information:
 *
 *  Initial investigation indicates a software managed stack will be needed.
 *  But the implementation does not currently include support for one.
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
 *  C4x Specific Information:
 *
 *  XXXanswer
 *
 *  Initial investigation indicates a software managed stack will be needed.
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
 *  C4x Specific Information:
 *
 *  XXXanswer
 *
 *  Until we know what to do with the memory, we should not allocated it.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 *
 *  The interrupt code will have to be written before this is answered
 *  but the answer should be yes.
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
 *  The macro name "C4X_HAS_FPU" should be made CPU specific.
 *  It indicates whether or not this CPU model has FP support.  For
 *  example, it would be possible to have an i386_nofp CPU model
 *  which set this to false to indicate that you have an i386 without
 *  an i387 and wish to leave floating point support out of RTEMS.
 *
 *  C4x Specific Information:
 *
 *  See c4x.h for more details but the bottom line is that the 
 *  few extended registers required to be preserved across subroutines
 *  calls are considered part of the integer context.  This eliminates
 *  overhead.
 *
 *  The C4X_HAS_FPU refers to the extended precision registers R0-R7
 *  (plus R8-R11 on some models).
 *
 *  XXX check that we even need to have the context area pointer in
 *  the TCB in this case.
 */

#if ( C4X_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif
#define CPU_SOFTWARE_FP     FALSE

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
 *  C4x Specific Information:
 *
 *  There is no known reason to make all tasks include the extended
 *  precision registers (i.e. floating point context).
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
 *  C4x Specific Information:
 *
 *  There is no known reason to make the IDLE task floating point and 
 *  no point in wasting the memory or increasing the context switch
 *  time for the IDLE task.
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
 *  C4x Specific Information:
 *
 *  There is no reason to avoid the deferred FP switch logic on this 
 *  CPU family.
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
 *  C4x Specific Information:
 *
 *  There is currently no reason to avoid using the generic implementation.
 *  In the future, a C4x specific IDLE thread body may be added to take
 *  advantage of low power modes.
 */

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 *
 *  C4x Specific Information:
 *
 *  The system stack grows from low to high memory.
 *
 *  C4x Specific Information:
 *
 *  This setting was derived from the discussion of stack management
 *  in section 6.1 (p. 6-29) System and User Stack Management of the
 *  TMS32C3x User's Guide (rev L, July 1997) which states: "A push
 *  performs a preincrement, and a pop performs a postdecrement of the
 *  system-stack pointer."  There are instructions for making "a stack"
 *  run from high to low memory but this appears to be the exception.
 */

#define CPU_STACK_GROWS_UP               TRUE

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
 *  C4x Specific Information:
 *
 *  The C4x is word oriented and there should be no alignment issues.
 */

#define CPU_STRUCTURE_ALIGNMENT

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 *
 *  C4x Specific Information:
 *
 */

#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 *  C4x Specific Information:
 *
 *  Currently we are only supporting interrupt levels 0 (all on) and
 *  1 (all off).  Levels 2-255 COULD be looked up in a user provided
 *  table that gives GIE and IE Mask settings.  But this is not the
 *  case today.
 */

#define CPU_MODES_INTERRUPT_MASK   0x000000FF

/*
 *  Processor defined structures
 *
 *  Examples structures include the descriptor tables from the i386
 *  and the processor control structure on the i960ca.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *
 *  C4x Specific Information:
 *
 *  From email with Michael Hayes:
 *  > > But what are the rules for what is passed in what registers?
 *  
 *  Args are passed in the following registers (in order):
 *  
 *  AR2, R2, R3, RC, RS, RE
 *  
 *  However, the first and second floating point values are always in R2
 *  and R3 (and all other floats are on the stack).  Structs are always
 *  passed on the stack.  If the last argument is an ellipsis, the
 *  previous argument is passed on the stack so that its address can be
 *  taken for the stdargs macros.
 *  
 *   > > What is assumed to be preserved across calls?
 *  
 *  AR3, AR4, AR5, AR6, AR7   
 *  R4, R5, R8              (using STI/LDI)
 *  R6, R7                  (using STF/LDF)
 *  
 *   > > What is assumed to be scratch registers?
 *  
 *  R0, R1, R2, R3, AR0, AR1, AR2, IR0, IR1, BK, RS, RE, RC, R9, R10, R11
 *  
 *  Based on this information, the task specific context is quite small
 *  but the interrupt context is much larger.  In fact, it could
 *  easily be argued that there is no point in distinguishing between
 *  integer and floating point contexts on the Cxx since there is
 *  so little context involved.  So that is the decision made.
 *
 *  Not Mentioned in list: DP
 *
 *  Assumed to be global resources:
 *
 *  C3X: IE, IF, and IOF
 *  C4X: DIE, IIF, and IIF
 */


typedef struct {
  unsigned int st;
  unsigned int ar3;
  unsigned int ar4;
  unsigned int ar5;
  unsigned int ar6;
  unsigned int ar7;
  unsigned int r4_sti;  /* other part of register is in interrupt context */
  unsigned int r5_sti;  /* other part of register is in interrupt context */
  unsigned int r6_stf;  /* other part of register is in interrupt context */
  unsigned int r7_stf;  /* other part of register is in interrupt context */
#ifdef _TMS320C40
  unsigned int r8_sti;  /* other part of register is in interrupt context */
#endif
  unsigned int sp;
} Context_Control;

typedef struct {
} Context_Control_fp;

/*
 *  This is the order the interrupt entry code pushes the registers.
 */

typedef struct {
  void        *interrupted;
  unsigned int st;
  unsigned int ar2;   /* because the vector numbers goes here */
  unsigned int ar0;
  unsigned int ar1;
  unsigned int dp;
  unsigned int ir0;
  unsigned int ir1;
  unsigned int rs;
  unsigned int re;
  unsigned int rc;
  unsigned int bk;
  unsigned int r0_sti;
  unsigned int r0_stf;
  unsigned int r1_sti;
  unsigned int r1_stf;
  unsigned int r2_sti;
  unsigned int r2_stf;
  unsigned int r3_sti;
  unsigned int r3_stf;
  unsigned int r4_stf;  /* other part of register is in basic context */
  unsigned int r5_stf;  /* other part of register is in basic context */
  unsigned int r6_sti;  /* other part of register is in basic context */
  unsigned int r7_sti;  /* other part of register is in basic context */

#ifdef _TMS320C40
  unsigned int r8_sti;  /* other part of register is in basic context */
  unsigned int r9_sti;
  unsigned int r9_stf;
  unsigned int r10_sti;
  unsigned int r10_stf;
  unsigned int r11_sti;
  unsigned int r11_stf;
#endif

} CPU_Interrupt_frame;

/*
 *  The following table contains the information required to configure
 *  the C4x processor specific parameters.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 */

/*
 *  Macros to access C4X specific additions to the CPU Table
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

/* There are no CPU specific additions to the CPU Table for this port. */

#if 0
/*
 *  This variable is optional.  It is used on CPUs on which it is difficult
 *  to generate an "uninitialized" FP context.  It is filled in by
 *  _CPU_Initialize and copied into the task's FP context area during
 *  _CPU_Context_Initialize.
 *
 *  C4x Specific Information:
 *
 *  Unused
 */

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context;
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
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *
 *  C4x Specific Information:
 *
 *  This port should not require this.
 */

#if 0
SCORE_EXTERN void           (*_CPU_Thread_dispatch_pointer)();
#endif

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

/* XXX: if needed, put more variables here */

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 *
 *  C4x Specific Information:
 *
 *  If we decide to have a separate floating point context, then
 *  the answer is the size of the data structure.  Otherwise, we
 *  need to define it as 0 to let upper level configuration work.
 */

#if ( C4X_HAS_FPU == 1 )
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )
#else
#define CPU_CONTEXT_FP_SIZE 0
#endif

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by RTEMS.
 *
 *  C4x Specific Information:
 *
 *  Based on the information provided in section 7.6.1 (p. 7-26) 
 *  titled "TMS320C30 and TMS320C31 Interrupt Vector Table" and section
 *  7.6.2 "TMS320C32 Interrupt Vector Table" of the TMS32C3x User's
 *  Guide (rev L, July 1997), vectors are numbered 0x00 - 0x3F.  Thus
 *  there are 0x40 or 64 vectors.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      0x40
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Should be large enough to run all RTEMS tests.  This insures
 *  that a "reasonable" small application should not have any problems.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#define CPU_STACK_MINIMUM_SIZE          (1024)

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 *  As best I can tell, there are no restrictions since this is a word
 *  -- not byte -- oriented archtiecture.
 */

#define CPU_ALIGNMENT              0

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
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 *
 *  A CPU_HEAP_ALIGNMENT of 2 comes close to disabling all the rounding
 *  while still ensuring that the least significant bit of the front
 *  and back flags can be used as the used bit -- not part of the size.
 */

#define CPU_HEAP_ALIGNMENT         2

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
 *  C4x Specific Information:
 *
 *  XXXanswer
 *  I think a CPU_PARTITION_ALIGNMENT of 1 will effectively disable all
 *  the rounding.
 */

#define CPU_PARTITION_ALIGNMENT    1

/*
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#define CPU_STACK_ALIGNMENT        0

/*
 *  ISR handler macros
 *
 *  C4x Specific Information:
 *
 *  These macros disable interrupts using the GIE (global interrupts enable)
 *  bit in the status word.
 */

/*
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 */

#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _isr_cookie.
 */

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    (_isr_cookie) = c4x_global_interrupts_get(); \
    c4x_global_interrupts_disable(); \
  } while (0)

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _isr_cookie is not modified.
 */

#define _CPU_ISR_Enable( _isr_cookie )  \
  c4x_global_interrupts_restore( _isr_cookie )

/*
 *  This temporarily restores the interrupt to _isr_cookie before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _isr_cookie is not
 *  modified.
 */

#define _CPU_ISR_Flash( _isr_cookie ) \
  c4x_global_interrupts_flash( _isr_cookie )

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
 *  C4x Specific Information:
 *
 *  The C4x port probably needs to allow the BSP to define
 *  a mask table for all values 0-255.  For now, 0 is global
 *  interrupts enabled and and non-zero is global interrupts
 *  disabled.  In the future, values 1-254 could be defined as
 *  specific combinations of the global interrupt enabled and the IE mask.
 *
 *  The logic for setting the mask field is something like this:
 *    _ie_value = c4x_get_ie();
 *    _ie_value &= C4X_IE_INTERRUPT_MASK_BITS;
 *    _ie_value |= _ie_mask;
 *    c4x_set_ie(_ie_value);
 *
 *  NOTE:  If this is implemented, then the context of each task
 *         must be extended to include the IE register.
 */

#define _CPU_ISR_Set_level( _new_level ) \
  do { \
    if ( _new_level == 0 ) c4x_global_interrupts_enable(); \
    else                   c4x_global_interrupts_disable(); \
  } while (0)

/* if GIE = 1, then logical level is 0. */
#define _CPU_ISR_Get_level() \
  (c4x_global_interrupts_get() ? 0 : 1)


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
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

void _CPU_Context_Initialize(
  Context_Control       *_the_context,
  void                  *_stack_base,
  unsigned32            _size,
  unsigned32            _isr,
  void  (*_entry_point)(void),
  int                   _is_fp
);

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#if ( C4X_HAS_FPU == 1 )
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
 *  C4x Specific Information:
 *
 *  No Floating Point from RTEMS perspective.
 */

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )
#endif

#if ( C4X_HAS_FPU == 1 )
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
 *  C4x Specific Information:
 *
 *  No Floating Point from RTEMS perspective.
 */

#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
   *((Context_Control_fp *) *((void **) _destination)) = _CPU_Null_fp_context; \
  } while (0)
#endif

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#define _CPU_Fatal_halt( _error ) \
  do { \
  } while (0)

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
 *  C4x Specific Information:
 *
 *  There does not appear to be a simple way to do this on this
 *  processor family that is better than the generic algorithm.
 *  Almost certainly, a hand-optimized assembly version of the
 *  generic algorithm could be written although it is not 
 *  worth the development effort at this time.
 */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  do { \
    (_output) = 0;   /* do something to prevent warnings */ \
  } while (0)

#endif

/* end of Bitfield handler macros */

/*
 *  This routine builds the mask which corresponds to the bit fields
 *  as searched by _CPU_Bitfield_Find_first_bit().  See the discussion
 *  for that routine.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

#endif

/*
 *  This routine translates the bit numbers returned by
 *  _CPU_Bitfield_Find_first_bit() into something suitable for use as
 *  a major or minor component of a priority.  See the discussion
 *  for that routine.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_Thread_Idle_body
 *
 *  This routine is the CPU dependent IDLE thread body.
 *
 *  NOTE:  It need only be provided if CPU_PROVIDES_IDLE_THREAD_BODY
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
 *         is TRUE.
 */

#if (CPU_PROVIDES_IDLE_THREAD_BODY == 1)
void _CPU_Thread_Idle_body( void );
#endif

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 *
 *  C4x Specific Information:
 *
 *  XXXanswer
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
 *  C4x Specific Information:
 *
 *  XXXanswer
 */

void _CPU_Context_restore(
  Context_Control *new_context
);

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 *
 *  C4x Specific Information:
 *
 *  No Floating Point from RTEMS perspective.
 */

#if ( C4X_HAS_FPU == 1 )
void _CPU_Context_save_fp(
  void **fp_context_ptr
);
#endif

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 *
 *  C4x Specific Information:
 *
 *  No Floating Point from RTEMS perspective.
 */

#if ( C4X_HAS_FPU == 1 )
void _CPU_Context_restore_fp(
  void **fp_context_ptr
);
#endif

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
 *  C4x Specific Information:
 *
 *  XXXanswer
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
