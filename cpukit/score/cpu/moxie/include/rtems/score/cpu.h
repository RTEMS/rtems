/**
 * @file
 * 
 * @addtogroup RTEMSScoreCPUMoxie
 */

/*
 *  This include file contains information pertaining to the Moxie
 *  processor.
 *
 *  Copyright (c) 2013  Anthony Green
 *
 *  Based on code with the following copyright..
 *  COPYRIGHT (c) 1989-2006, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/moxie.h>  /* pick up machine definitions */

#include <rtems/bspIo.h>        /* printk */

/* conditional compilation parameters */

/*
 *  Should this target use 16 or 32 bit object Ids?
 *
 */
#define RTEMS_USE_32_BIT_OBJECT

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  MOXIE Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/*
 *  Does the CPU have hardware floating point?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is supported.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is ignored.
 *
 *  If there is a FP coprocessor such as the i387 or mc68881, then
 *  the answer is TRUE.
 *
 *  The macro name "MOXIE_HAS_FPU" should be made CPU specific.
 *  It indicates whether or not this CPU model has FP support.  For
 *  example, it would be possible to have an i386_nofp CPU model
 *  which set this to false to indicate that you have an i386 without
 *  an i387 and wish to leave floating point support out of RTEMS.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_HARDWARE_FP     FALSE

/*
 *  Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
 *
 *  If CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_STACK_GROWS_UP               FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 32

#define CPU_STRUCTURE_ALIGNMENT

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_MODES_INTERRUPT_MASK   0x00000001

#define CPU_MAXIMUM_PROCESSORS 32

/*
 *  Processor defined structures required for cpukit/score.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  MOXIE Specific Information:
 *
 *  XXX
 */

#define nogap __attribute__ ((packed))

typedef struct {
    void        *fp nogap;
    void        *sp nogap;
    uint32_t    r0 nogap;
    uint32_t    r1 nogap;
    uint32_t    r2 nogap;
    uint32_t    r3 nogap;
    uint32_t    r4 nogap;
    uint32_t    r5 nogap;
    uint32_t    r6 nogap;
    uint32_t    r7 nogap;
    uint32_t    r8 nogap;
    uint32_t    r9 nogap;
    uint32_t    r10 nogap;
    uint32_t    r11 nogap;
    uint32_t    r12 nogap;
    uint32_t    r13 nogap;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->sp

typedef struct {
    double      some_float_register[2];
} Context_Control_fp;

typedef struct {
    uint32_t   special_interrupt_register;
} CPU_Interrupt_frame;

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  system initialization thread.  Remember that in a multiprocessor
 *  system the system intialization thread becomes the MP server thread.
 *
 *  MOXIE Specific Information:
 *
 *  It is highly unlikely the MOXIE will get used in a multiprocessor system.
 */
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by RTEMS.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS      64
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER \
    (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Should be large enough to run all RTEMS tests.  This ensures
 *  that a "reasonable" small application should not have any problems.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_STACK_MINIMUM_SIZE          (1536)

/**
 * Size of a pointer.
 *
 * This must be an integer literal that can be used by the assembler.  This
 * value will be used to calculate offsets of structure members.  These
 * offsets will be used in assembler code.
 */
#define CPU_SIZEOF_POINTER         4

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define CPU_STACK_ALIGNMENT        0

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  ISR handler macros
 */

/*
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 */
#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 *
 *  MOXIE Specific Information:
 *
 *  TODO: As of 7 October 2014, this method is not implemented.
 */
#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    (_isr_cookie) = 0; \
  } while (0)

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _level is not modified.
 *
 *  MOXIE Specific Information:
 *
 *  TODO: As of 7 October 2014, this method is not implemented.
 */
#define _CPU_ISR_Enable( _isr_cookie ) \
  do { \
    (_isr_cookie) = (_isr_cookie); \
  } while (0)

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 *
 *  MOXIE Specific Information:
 *
 *  TODO: As of 7 October 2014, this method is not implemented.
 */
#define _CPU_ISR_Flash( _isr_cookie ) \
  do { \
    _CPU_ISR_Enable( _isr_cookie ); \
    _CPU_ISR_Disable( _isr_cookie ); \
  } while (0)

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return true;
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
 *  MOXIE Specific Information:
 *
 *  TODO: As of 7 October 2014, this method is not implemented.
 */
#define _CPU_ISR_Set_level( _new_level )        \
  {                                                     \
    if (_new_level)   asm volatile ( "nop\n" );         \
    else              asm volatile ( "nop\n" );         \
  }

uint32_t   _CPU_ISR_Get_level( void );

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
 *  MOXIE Specific Information:
 *
 *  TODO: As of 7 October 2014, this method does not ensure that the context
 *  is set up with interrupts disabled/enabled as requested.
 */
#define CPU_CCR_INTERRUPTS_ON  0x80
#define CPU_CCR_INTERRUPTS_OFF 0x00

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp, _tls_area ) \
  /* Locate Me */                                                  \
  do {                                                             \
    uintptr_t   _stack;                                            \
                                                                   \
    (void) _is_fp; /* avoid warning for being unused */            \
    (void) _isr;   /* avoid warning for being unused */            \
    _stack = ((uintptr_t)(_stack_base)) + (_size) - 8;             \
    *((void (**)(void))(_stack)) = (_entry_point);                 \
    _stack -= 4;                                                   \
    (_the_context)->fp = (void *)_stack;                           \
    (_the_context)->sp = (void *)_stack;                           \
  } while (0)


/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Initialize_fp( _destination ) \
  memset( *( _destination ), 0, CPU_CONTEXT_FP_SIZE );

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
#define _CPU_Fatal_halt( _source, _error ) \
        printk("Fatal Error %d.%lu Halted\n",_source,_error); \
        for(;;)

/* end of Fatal Error manager macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
void _CPU_Initialize(void);

typedef void ( *CPU_ISR_handler )( uint32_t );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

void *_CPU_Thread_Idle_body( uintptr_t );

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
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
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_NO_RETURN;

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 *
 *  MOXIE Specific Information:
 *
 *  XXX
 */
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

/**
 * @brief The set of registers that specifies the complete processor state.
 *
 * The CPU exception frame may be available in fatal error conditions like for
 * example illegal opcodes, instruction fetch errors, or data access errors.
 *
 * @see rtems_fatal(), RTEMS_FATAL_SOURCE_EXCEPTION, and
 * rtems_exception_frame_print().
 */
typedef struct {
  uint32_t integer_registers [16];
} CPU_Exception_frame;

/**
 * @brief Prints the exception frame via printk().
 *
 * @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
 */
void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

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
 *
 *  MOXIE Specific Information:
 *
 *  This is the generic implementation.
 */
static inline uint32_t   CPU_swap_u32(
  uint32_t   value
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

#ifdef __cplusplus
}
#endif

#endif
