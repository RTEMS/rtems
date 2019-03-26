/**
 * @file
 *
 * @ingroup RTEMScoreCPUor1k
 */

/*
 *  This include file contains macros pertaining to the Opencores
 *  or1k processor family.
 *
 *  COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  This file adapted from no_cpu example of the RTEMS distribution.
 *  The body has been modified for the Opencores OR1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 *
 */

#ifndef _OR1K_CPU_H
#define _OR1K_CPU_H

#ifdef __cplusplus
extern "C" {
#endif


#include <rtems/score/or1k.h>            /* pick up machine definitions */
#include <rtems/score/or1k-utility.h>
#include <rtems/score/basedefs.h>
#ifndef ASM
#include <rtems/bspIo.h>
#include <stdint.h>
#include <stdio.h> /* for printk */
#endif

/* conditional compilation parameters */

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector
 *  number (0)?
 *
 */

#define CPU_ISR_PASSES_FRAME_POINTER TRUE

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

#define CPU_HARDWARE_FP     FALSE
#define CPU_SOFTWARE_FP     FALSE

/*
 *  Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
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

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 *
 */

#define CPU_STACK_GROWS_UP               FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 32

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 */

#define CPU_MODES_INTERRUPT_MASK   0x00000001

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
 *
 *
 */
#ifndef ASM
#ifdef OR1K_64BIT_ARCH
#define or1kreg uint64_t
#else
#define or1kreg uint32_t
#endif

typedef struct {
  uint32_t  r1;     /* Stack pointer */
  uint32_t  r2;     /* Frame pointer */
  uint32_t  r3;
  uint32_t  r4;
  uint32_t  r5;
  uint32_t  r6;
  uint32_t  r7;
  uint32_t  r8;
  uint32_t  r9;
  uint32_t  r10;
  uint32_t  r11;
  uint32_t  r12;
  uint32_t  r13;
  uint32_t  r14;
  uint32_t  r15;
  uint32_t  r16;
  uint32_t  r17;
  uint32_t  r18;
  uint32_t  r19;
  uint32_t  r20;
  uint32_t  r21;
  uint32_t  r22;
  uint32_t  r23;
  uint32_t  r24;
  uint32_t  r25;
  uint32_t  r26;
  uint32_t  r27;
  uint32_t  r28;
  uint32_t  r29;
  uint32_t  r30;
  uint32_t  r31;

  uint32_t  sr;  /* Current supervision register non persistent values */
  uint32_t  epcr;
  uint32_t  eear;
  uint32_t  esr;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->r1

typedef struct {
  /** FPU registers are listed here */
  double      some_float_register;
} Context_Control_fp;

typedef Context_Control CPU_Interrupt_frame;

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 *
 *  Or1k Specific Information:
 *
 */

#define CPU_CONTEXT_FP_SIZE  0

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 *
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  Should be large enough to run all RTEMS tests.  This insures
 *  that a "reasonable" small application should not have any problems.
 *
 */

#define CPU_STACK_MINIMUM_SIZE  4096

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 */

#define CPU_ALIGNMENT  8

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/**
 * Size of a pointer.
 *
 * This must be an integer literal that can be used by the assembler.  This
 * value will be used to calculate offsets of structure members.  These
 * offsets will be used in assembler code.
 */
#define CPU_SIZEOF_POINTER         4

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
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 *
 */

#define CPU_STACK_ALIGNMENT        0

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/* ISR handler macros */

/*
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 *
 */

static inline uint32_t or1k_interrupt_disable( void )
{
  uint32_t sr;
  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);

  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_IEE));

  return sr;
}

static inline void or1k_interrupt_enable(uint32_t level)
{
  uint32_t sr;

  /* Enable interrupts and restore rs */
  sr = level | CPU_OR1K_SPR_SR_IEE | CPU_OR1K_SPR_SR_TEE;
  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr);

}

#define _CPU_ISR_Disable( _level ) \
    _level = or1k_interrupt_disable()


/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _level is not modified.
 *
 */

#define _CPU_ISR_Enable( _level )  \
  or1k_interrupt_enable( _level )

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 *
 */

#define _CPU_ISR_Flash( _level ) \
  do{ \
      _CPU_ISR_Enable( _level ); \
      _OR1K_mtspr(CPU_OR1K_SPR_SR, (_level & ~CPU_OR1K_SPR_SR_IEE)); \
    } while(0)

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return ( level & CPU_OR1K_SPR_SR ) != 0;
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

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/* Context handler macros */

#define OR1K_FAST_CONTEXT_SWITCH_ENABLED FALSE
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

/**
 * @brief Initializes the CPU context.
 *
 * The following steps are performed:
 *  - setting a starting address
 *  - preparing the stack
 *  - preparing the stack and frame pointers
 *  - setting the proper interrupt level in the context
 *
 * @param[in] context points to the context area
 * @param[in] stack_area_begin is the low address of the allocated stack area
 * @param[in] stack_area_size is the size of the stack area in bytes
 * @param[in] new_level is the interrupt level for the task
 * @param[in] entry_point is the task's entry point
 * @param[in] is_fp is set to @c true if the task is a floating point task
 * @param[in] tls_area is the thread-local storage (TLS) area
 */
void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
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
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/*
 *  This routine is responsible to initialize the FP context.
 *
 *  The FP area pointer is passed by reference to allow the initial pointer
 *  into a floating point context area (used to save the floating point
 *  context) to be at an arbitrary place in the floating point context area.
 *
 *  This is necessary because some FP units are designed to have
 *  their context saved as a stack which grows into lower addresses.
 *  Other FP units can be saved by simply moving registers into offsets
 *  from the base of the context area.  Finally some FP units provide
 *  a "dump context" instruction which could fill in from high to low
 *  or low to high based on the whim of the CPU designers.
 */
#define _CPU_Context_Initialize_fp( _fp_area_p ) \
  memset( *( _fp_area_p ), 0, CPU_CONTEXT_FP_SIZE )

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 *
 */

#include <inttypes.h>

#define _CPU_Fatal_halt(_source, _error ) \
        printk("Fatal Error %d.%" PRId32 " Halted\n",_source, _error); \
        _OR1KSIM_CPU_Halt(); \
        for(;;)

/* end of Fatal Error manager macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#endif /* ASM */

#define CPU_SIZEOF_POINTER 4

#define CPU_MAXIMUM_PROCESSORS 32

#ifndef ASM
typedef struct {
  uint32_t r[32];

  /* The following registers must be saved if we have
  fast context switch disabled and nested interrupt
  levels are enabled.
  */
#if !OR1K_FAST_CONTEXT_SWITCH_ENABLED
  uint32_t epcr; /* exception PC register */
  uint32_t eear; /* exception effective address register */
  uint32_t esr; /* exception supervision register */
#endif

} CPU_Exception_frame;

/**
 * @brief Prints the exception frame via printk().
 *
 * @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
 */
void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );


/* end of Priority handler macros */

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 *
 */

void _CPU_Initialize(
  void
);

typedef void ( *CPU_ISR_raw_handler )( uint32_t, CPU_Exception_frame * );

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
);

typedef void ( *CPU_ISR_handler )( uint32_t );

RTEMS_INLINE_ROUTINE void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  _CPU_ISR_install_raw_handler(
    vector,
    (CPU_ISR_raw_handler) new_handler,
    (CPU_ISR_raw_handler *) old_handler
  );
}

void *_CPU_Thread_Idle_body( uintptr_t ignored );

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
) RTEMS_NO_RETURN;

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
  uint32_t   byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

static inline CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
