/**
 * @file
 *
 * @brief V850 CPU Department Source
 *
 * This include file contains information pertaining to the v850
 * processor.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <rtems/score/v850.h>

/* conditional compilation parameters */

/**
 * Does the CPU follow the simple vectored interrupt model?
 *
 * If TRUE, then RTEMS allocates the vector table it internally manages.
 * If FALSE, then the BSP is assumed to allocate and manage the vector
 * table
 *
 * Port Specific Information:
 *
 * This port uses the Progammable Interrupt Controller interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

/**
 * @def CPU_HARDWARE_FP
 *
 * Does the CPU have hardware floating point?
 *
 * If TRUE, then the RTEMS_FLOATING_POINT task attribute is supported.
 * If FALSE, then the RTEMS_FLOATING_POINT task attribute is ignored.
 *
 * If there is a FP coprocessor such as the i387 or mc68881, then
 * the answer is TRUE.
 *
 * The macro name "V850_HAS_FPU" should be made CPU specific.
 * It indicates whether or not this CPU model has FP support.  For
 * example, it would be possible to have an i386_nofp CPU model
 * which set this to false to indicate that you have an i386 without
 * an i387 and wish to leave floating point support out of RTEMS.
 */

/**
 * @def CPU_SOFTWARE_FP
 *
 * Does the CPU have no hardware floating point and GCC provides a
 * software floating point implementation which must be context
 * switched?
 *
 * This feature conditional is used to indicate whether or not there
 * is software implemented floating point that must be context
 * switched.  The determination of whether or not this applies
 * is very tool specific and the state saved/restored is also
 * compiler specific.
 *
 * Port Specific Information:
 *
 * Some v850 models do have IEEE hardware floating point support but
 * they do not have any special registers to save or bit(s) which
 * determine if the FPU is enabled. In short, there appears to be nothing
 * related to the floating point operations which impact the RTEMS
 * thread context switch. Thus from an RTEMS perspective, there is really
 * no FPU to manage.
 */
#define CPU_HARDWARE_FP     FALSE
#define CPU_SOFTWARE_FP     FALSE

/**
 * Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 * If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 * If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
 *
 * So far, the only CPUs in which this option has been used are the
 * HP PA-RISC and PowerPC.  On the PA-RISC, The HP C compiler and
 * gcc both implicitly used the floating point registers to perform
 * integer multiplies.  Similarly, the PowerPC port of gcc has been
 * seen to allocate floating point local variables and touch the FPU
 * even when the flow through a subroutine (like vfprintf()) might
 * not use floating point formats.
 *
 * If a function which you would not think utilize the FP unit DOES,
 * then one can not easily predict which tasks will use the FP hardware.
 * In this case, this option should be TRUE.
 *
 * If @ref CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.
 *
 * Port Specific Information:
 *
 * This should be false until it has been demonstrated that gcc for the
 * v850 generates FPU code when it is unexpected. But even this would
 * not matter since there are no FP specific registers or bits which
 * would be corrupted if an FP operation occurred in an integer only
 * thread.
 */
#define CPU_ALL_TASKS_ARE_FP     FALSE

/**
 * Should the IDLE task have a floating point context?
 *
 * If TRUE, then the IDLE task is created as a RTEMS_FLOATING_POINT task
 * and it has a floating point context which is switched in and out.
 * If FALSE, then the IDLE task does not have a floating point context.
 *
 * Setting this to TRUE negatively impacts the time required to preempt
 * the IDLE task from an interrupt because the floating point context
 * must be saved as part of the preemption.
 *
 * Port Specific Information:
 *
 * The IDLE thread should not be using the FPU. Leave this off.
 */
#define CPU_IDLE_TASK_IS_FP      FALSE

/**
 * Should the saving of the floating point registers be deferred
 * until a context switch is made to another different floating point
 * task?
 *
 * If TRUE, then the floating point context will not be stored until
 * necessary.  It will remain in the floating point registers and not
 * disturned until another floating point task is switched to.
 *
 * If FALSE, then the floating point context is saved when a floating
 * point task is switched out and restored when the next floating point
 * task is restored.  The state of the floating point registers between
 * those two operations is not specified.
 *
 * If the floating point context does NOT have to be saved as part of
 * interrupt dispatching, then it should be safe to set this to TRUE.
 *
 * Setting this flag to TRUE results in using a different algorithm
 * for deciding when to save and restore the floating point context.
 * The deferred FP switch algorithm minimizes the number of times
 * the FP context is saved and restored.  The FP context is not saved
 * until a context switch is made to another, different FP task.
 * Thus in a system with only one FP task, the FP context will never
 * be saved or restored.
 *
 * Port Specific Information:
 *
 * See earlier comments. There is no FPU state to manage.
 */
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE

/**
 * Does the stack grow up (toward higher addresses) or down
 * (toward lower addresses)?
 *
 * If TRUE, then the grows upward.
 * If FALSE, then the grows toward smaller addresses.
 *
 * Port Specific Information:
 *
 * The v850 stack grows from high addresses to low addresses.
 */
#define CPU_STACK_GROWS_UP               FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 32

#define CPU_STRUCTURE_ALIGNMENT

/**
 * @addtogroup RTEMSScoreCPUV850CPUInterrupt
 * The following defines the number of bits actually used in the
 * interrupt field of the task mode.  How those bits map to the
 * CPU interrupt levels is defined by the routine @ref _CPU_ISR_Set_level.
 *
 * Port Specific Information:
 *
 * The v850 only has a single bit in the CPU for interrupt disable/enable.
 */
#define CPU_MODES_INTERRUPT_MASK   0x00000001

#define CPU_MAXIMUM_PROCESSORS 32

/**
 * @defgroup RTEMSScoreCPUV850CPUContext Processor Dependent Context Management
 * 
 * @ingroup RTEMSScoreCPUV850
 *
 * From the highest level viewpoint, there are 2 types of context to save.
 *
 *    -# Interrupt registers to save
 *    -# Task level registers to save
 *
 * Since RTEMS handles integer and floating point contexts separately, this
 * means we have the following 3 context items:
 *
 *    -# task level context stuff::  Context_Control
 *    -# floating point task stuff:: Context_Control_fp
 *    -# special interrupt level context :: CPU_Interrupt_frame
 *
 * On some processors, it is cost-effective to save only the callee
 * preserved registers during a task context switch.  This means
 * that the ISR code needs to save those registers which do not
 * persist across function calls.  It is not mandatory to make this
 * distinctions between the caller/callee saves registers for the
 * purpose of minimizing context saved during task switch and on interrupts.
 * If the cost of saving extra registers is minimal, simplicity is the
 * choice.  Save the same context on interrupt entry as for tasks in
 * this case.
 *
 * Additionally, if gdb is to be made aware of RTEMS tasks for this CPU, then
 * care should be used in designing the context area.
 *
 * On some CPUs with hardware floating point support, the Context_Control_fp
 * structure will not be used or it simply consist of an array of a
 * fixed number of bytes.   This is done when the floating point context
 * is dumped by a "FP save context" type instruction and the format
 * is not really defined by the CPU.  In this case, there is no need
 * to figure out the exact format -- only the size.  Of course, although
 * this is enough information for RTEMS, it is probably not enough for
 * a debugger such as gdb.  But that is another problem.
 *
 * Port Specific Information:
 *
 * On the v850, this port saves special registers and those that are
 * callee saved.
 */
/** @{ **/

/**
 * This defines the minimal set of integer and processor state registers
 * that must be saved during a voluntary context switch from one thread
 * to another.
 */
typedef struct {
    uint32_t   r1;
    /** This field is the stack pointer (e.g. r3).  */
    uint32_t   r3_stack_pointer;
    uint32_t   r20;
    uint32_t   r21;
    uint32_t   r22;
    uint32_t   r23;
    uint32_t   r24;
    uint32_t   r25;
    uint32_t   r26;
    uint32_t   r27;
    uint32_t   r28;
    uint32_t   r29;
    uint32_t   r31;
    uint32_t   psw;
} Context_Control;

/**
 * This macro returns the stack pointer associated with @a _context.
 *
 * @param[in] _context is the thread context area to access
 *
 * @return This method returns the stack pointer.
 */
#define _CPU_Context_Get_SP( _context ) \
  (_context)->r3_stack_pointer

/**
 * This defines the complete set of floating point registers that must
 * be saved during any context switch from one thread to another.
 */
typedef struct {
    /** FPU registers are listed here */
    double      some_float_register;
} Context_Control_fp;

/**
 * This defines the set of integer and processor state registers that must
 * be saved during an interrupt.  This set does not include any which are
 * in @ref Context_Control.
 */
typedef struct {
    /** This field is a hint that a port will have a number of integer
     * registers that need to be saved when an interrupt occurs or
     * when a context switch occurs at the end of an ISR.
     */
    uint32_t   special_interrupt_register;
} CPU_Interrupt_frame;

/**
 * The size of the floating point context area.  On some CPUs this
 * will not be a "sizeof" because the format of the floating point
 * area is not defined -- only the size is.  This is usually on
 * CPUs with a "floating point save context" instruction.
 *
 * Port Specific Information:
 *
 * The v850 does not need a floating point context but this needs to be
 * defined so confdefs.h.
 */
/* #define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp ) */
#define CPU_CONTEXT_FP_SIZE 0

/** @} */

/**
 * @defgroup RTEMSScoreCPUV850CPUInterrupt Processor Dependent Interrupt Management
 * 
 * @ingroup RTEMSScoreCPUV850
 */
/** @{ **/

/**
 * Amount of extra stack (above minimum stack size) required by
 * MPCI receive server thread.  Remember that in a multiprocessor
 * system this thread must exist and be able to process all directives.
 *
 * Port Specific Information:
 *
 * There is no reason to think the v850 needs extra MPCI receive
 * server stack.
 */
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/**
 * This is defined if the port has a special way to report the ISR nesting
 * level.  Most ports maintain the variable @a _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/** @} */

/**
 * @addtogroup RTEMSScoreCPUV850CPUContext
 * Should be large enough to run all RTEMS tests.  This ensures
 * that a "reasonable" small application should not have any problems.
 *
 * Port Specific Information:
 *
 * This should be very conservative on the v850.
 */
#define CPU_STACK_MINIMUM_SIZE          (1024*4)

#define CPU_SIZEOF_POINTER 4

/**
 * CPU's worst alignment requirement for data types on a byte boundary.  This
 * alignment does not take into account the requirements for the stack.
 *
 * Port Specific Information:
 *
 * There is no apparent reason why this should be larger than 8.
 */
#define CPU_ALIGNMENT              8

/**
 * This number corresponds to the byte alignment requirement for the
 * heap handler.  This alignment requirement may be stricter than that
 * for the data types alignment specified by @ref CPU_ALIGNMENT.  It is
 * common for the heap to follow the same alignment requirement as
 * @ref CPU_ALIGNMENT.  If the @ref CPU_ALIGNMENT is strict enough for
 * the heap, then this should be set to @ref CPU_ALIGNMENT.
 *
 * @note  This does not have to be a power of 2 although it should be
 *        a multiple of 2 greater than or equal to 2.  The requirement
 *        to be a multiple of 2 is because the heap uses the least
 *        significant field of the front and back flags to indicate
 *        that a block is in use or free.  So you do not want any odd
 *        length blocks really putting length data in that bit.
 *
 *        On byte oriented architectures, @ref CPU_HEAP_ALIGNMENT normally will
 *        have to be greater or equal to than @ref CPU_ALIGNMENT to ensure that
 *        elements allocated from the heap meet all restrictions.
 *
 * Port Specific Information:
 *
 * There is no apparent reason why this should be larger than CPU_ALIGNMENT.
 */
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

/**
 * This number corresponds to the byte alignment requirement for the
 * stack.  This alignment requirement may be stricter than that for the
 * data types alignment specified by @ref CPU_ALIGNMENT.  If the
 * @ref CPU_ALIGNMENT is strict enough for the stack, then this should be
 * set to 0.
 *
 * @note This must be a power of 2 either 0 or greater than @ref CPU_ALIGNMENT.
 *
 * Port Specific Information:
 *
 * The v850 has enough RAM where alignment to 16 may be desirable depending
 * on the cache properties. But this remains to be demonstrated.
 */
#define CPU_STACK_ALIGNMENT        4

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  ISR handler macros
 */

/**
 * @addtogroup RTEMSScoreCPUV850CPUInterrupt
 */
/** @{ **/

/**
 * Disable all interrupts for an RTEMS critical section.  The previous
 * level is returned in @a _isr_cookie.
 *
 * @param[out] _isr_cookie will contain the previous level cookie
 *
 * Port Specific Information:
 *
 * On the v850, we need to save the PSW and use "di" to disable interrupts.
 */
#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    unsigned int _psw; \
    \
    v850_get_psw( _psw ); \
    __asm__ __volatile__( "di" ); \
    _isr_cookie = _psw; \
  } while (0)

/**
 * Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 * This indicates the end of an RTEMS critical section.  The parameter
 * @a _isr_cookie is not modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * On the v850, we simply need to restore the PSW.
 */
#define _CPU_ISR_Enable( _isr_cookie )  \
  do { \
    unsigned int _psw = (_isr_cookie); \
    \
    v850_set_psw( _psw ); \
  } while (0)

/**
 * This temporarily restores the interrupt to @a _isr_cookie before immediately
 * disabling them again.  This is used to divide long RTEMS critical
 * sections into two or more parts.  The parameter @a _isr_cookie is not
 * modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * This saves at least one instruction over using enable/disable back to back.
 */
#define _CPU_ISR_Flash( _isr_cookie ) \
  do { \
    unsigned int _psw = (_isr_cookie); \
    v850_set_psw( _psw ); \
    __asm__ __volatile__( "di" ); \
  } while (0)

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return ( level & V850_PSW_INTERRUPT_DISABLE_MASK )
    != V850_PSW_INTERRUPT_DISABLE;
}

/**
 * This routine and @ref _CPU_ISR_Get_level
 * Map the interrupt level in task mode onto the hardware that the CPU
 * actually provides.  Currently, interrupt levels which do not
 * map onto the CPU in a generic fashion are undefined.  Someday,
 * it would be nice if these were "mapped" by the application
 * via a callout.  For example, m68k has 8 levels 0 - 7, levels
 * 8 - 255 would be available for bsp/application specific meaning.
 * This could be used to manage a programmable interrupt controller
 * via the rtems_task_mode directive.
 *
 * Port Specific Information:
 *
 * On the v850, level 0 is enabled. Non-zero is disabled.
 */
#define _CPU_ISR_Set_level( new_level ) \
  do { \
    if ( new_level ) \
      __asm__ __volatile__( "di" ); \
    else \
      __asm__ __volatile__( "ei" ); \
  } while (0)

/**
 * Return the current interrupt disable level for this task in
 * the format used by the interrupt level portion of the task mode.
 *
 * @note This routine usually must be implemented as a subroutine.
 *
 * Port Specific Information:
 *
 * This method is implemented in C on the v850.
 */
uint32_t   _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/** @} */

/* Context handler macros */

/**
 * @addtogroup RTEMSScoreCPUV850CPUContext
 * Initialize the context to a state suitable for starting a
 * task after a context restore operation.  Generally, this
 * involves:
 *
 *    - setting a starting address
 *    - preparing the stack
 *    - preparing the stack and frame pointers
 *    - setting the proper interrupt level in the context
 *    - initializing the floating point context
 *
 * This routine generally does not set any unnecessary register
 * in the context.  The state of the "general data" registers is
 * undefined at task start time.
 *
 * @param[in] _the_context is the context structure to be initialized
 * @param[in] _stack_base is the lowest physical address of this task's stack
 * @param[in] _size is the size of this task's stack
 * @param[in] _isr is the interrupt disable level
 * @param[in] _entry_point is the thread's entry point.  This is
 *        always @a _Thread_Handler
 * @param[in] _is_fp is TRUE if the thread is to be a floating
 *       point thread.  This is typically only used on CPUs where the
 *       FPU may be easily disabled by software such as on the SPARC
 *       where the PSR contains an enable FPU bit.
 * @param[in] tls_area is the thread-local storage (TLS) area
 *
 * Port Specific Information:
 *
 * This method is implemented in C on the v850.
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
);

/**
 * This routine is responsible for somehow restarting the currently
 * executing task.  If you are lucky, then all that is necessary
 * is restoring the context.  Otherwise, there will need to be
 * a special assembly routine which does something special in this
 * case.  For many ports, simply adding a label to the restore path
 * of @ref _CPU_Context_switch will work.  On other ports, it may be
 * possibly to load a few arguments and jump to the restore path. It will
 * not work if restarting self conflicts with the stack frame
 * assumptions of restoring a context.
 *
 * Port Specific Information:
 *
 * On the v850, we require a special entry point to restart a task.
 */
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/* XXX this should be possible to remove */
#if 0
/**
 * This routine initializes the FP context area passed to it to.
 * There are a few standard ways in which to initialize the
 * floating point context.  The code included for this macro assumes
 * that this is a CPU in which a "initial" FP context was saved into
 * @a _CPU_Null_fp_context and it simply copies it to the destination
 * context passed to it.
 *
 * Other floating point context save/restore models include:
 *   -# not doing anything, and
 *   -# putting a "null FP status word" in the correct place in the FP context.
 *
 * @param[in] _destination is the floating point context area
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Initialize_fp( _destination ) \
  { \
  }
#endif

/* end of Context handler macros */

/* Fatal Error manager macros */

/**
 * This routine copies _error into a known place -- typically a stack
 * location or a register, optionally disables interrupts, and
 * halts/stops the CPU.
 *
 * Port Specific Information:
 *
 * Move the error code into r10, disable interrupts and halt.
 */
#define _CPU_Fatal_halt( _source, _error ) \
  do { \
    __asm__ __volatile__ ( "di" ); \
    __asm__ __volatile__ ( "mov %0, r10; " : "=r" ((_error)) ); \
    __asm__ __volatile__ ( "halt" ); \
  } while (0)

/* end of Fatal Error manager macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

/* functions */

/**
 * @brief CPU initialize.
 * This routine performs CPU dependent initialization.
 *
 * Port Specific Information:
 *
 * This is implemented in C.
 *
 * v850 CPU Dependent Source
 */
void _CPU_Initialize(void);

void *_CPU_Thread_Idle_body( uintptr_t ignored );

/**
 * @addtogroup RTEMSScoreCPUV850CPUContext
 */
/**@{**/

/**
 * This routine switches from the run context to the heir context.
 *
 * @param[in] run points to the context of the currently executing task
 * @param[in] heir points to the context of the heir task
 *
 * Port Specific Information:
 *
 * This is implemented in assembly on the v850.
 */
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/**
 * This routine is generally used only to restart self in an
 * efficient manner.  It may simply be a label in @ref _CPU_Context_switch.
 *
 * @param[in] new_context points to the context to be restored.
 *
 * @note May be unnecessary to reload some registers.
 *
 * Port Specific Information:
 *
 * This is implemented in assembly on the v850.
 */
void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_NO_RETURN;

/* XXX this should be possible to remove */
#if 0
/**
 * This routine saves the floating point context passed to it.
 *
 * @param[in] fp_context_ptr is a pointer to a pointer to a floating
 * point context area
 *
 * @return on output @a *fp_context_ptr will contain the address that
 * should be used with @ref _CPU_Context_restore_fp to restore this context.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);
#endif

/* XXX this should be possible to remove */
#if 0
/**
 * This routine restores the floating point context passed to it.
 *
 * @param[in] fp_context_ptr is a pointer to a pointer to a floating
 * point context area to restore
 *
 * @return on output @a *fp_context_ptr will contain the address that
 * should be used with @ref _CPU_Context_save_fp to save this context.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);
#endif

/** @} */

/* FIXME */
typedef CPU_Interrupt_frame CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

/**
 * @defgroup RTEMSScoreCPUV850CPUEndian CPUEndian
 * 
 * @ingroup RTEMSScoreCPUV850
 * 
 * @brief CPUEndian
 */
/** @{ */

/**
 * The following routine swaps the endian format of an unsigned int.
 * It must be static because it is referenced indirectly.
 *
 * This version will work on any processor, but if there is a better
 * way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *    swap least significant two bytes with 16-bit rotate
 *    swap upper and lower 16-bits
 *    swap most significant two bytes with 16-bit rotate
 *
 * Some CPUs have special instructions which swap a 32-bit quantity in
 * a single instruction (e.g. i486).  It is probably best to avoid
 * an "endian swapping control bit" in the CPU.  One good reason is
 * that interrupts would probably have to be disabled to ensure that
 * an interrupt does not try to access the same "chunk" with the wrong
 * endian.  Another good reason is that on some CPUs, the endian bit
 * endianness for ALL fetches -- both code and data -- so the code
 * will be fetched incorrectly.
 *
 * @param[in] value is the value to be swapped
 * @return the value after being endian swapped
 *
 * Port Specific Information:
 *
 * The v850 has a single instruction to swap endianness on a 32 bit quantity.
 */
static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  unsigned int swapped;

  #if (V850_HAS_BYTE_SWAP_INSTRUCTION == 1)
    unsigned int v;

    v = value;
    __asm__ __volatile__ ("bsw %0, %1" : "=r" (v), "=&r" (swapped) );
  #else
    uint32_t byte1, byte2, byte3, byte4;

    byte4 = (value >> 24) & 0xff;
    byte3 = (value >> 16) & 0xff;
    byte2 = (value >> 8)  & 0xff;
    byte1 =  value        & 0xff;

    swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  #endif
  return swapped;
}

/**
 * This routine swaps a 16 bir quantity.
 *
 * @param[in] value is the value to be swapped
 * @return the value after being endian swapped
 *
 * Port Specific Information:
 *
 * The v850 has a single instruction to swap endianness on a 16 bit quantity.
 */
static inline uint16_t CPU_swap_u16( uint16_t value )
{
  unsigned int swapped;

  #if (V850_HAS_BYTE_SWAP_INSTRUCTION == 1)
    unsigned int v;

    v = value;
    __asm__ __volatile__ ("bsh %0, %1" : "=r" (v), "=&r" (swapped) );
  #else
    swapped = ((value & 0xff) << 8) | ((value >> 8) & 0xff);
  #endif
  return swapped;
}

/** @} */

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
