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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>
#include <rtems/score/v850.h>

/* conditional compilation parameters */

/**
 * Should the calls to @ref _Thread_Enable_dispatch be inlined?
 *
 * If TRUE, then they are inlined.
 * If FALSE, then a subroutine call is made.
 *
 * This conditional is an example of the classic trade-off of size
 * versus speed.  Inlining the call (TRUE) typically increases the
 * size of RTEMS while speeding up the enabling of dispatching.
 *
 * @note In general, the @ref _Thread_Dispatch_disable_level will
 * only be 0 or 1 unless you are in an interrupt handler and that
 * interrupt handler invokes the executive.]  When not inlined
 * something calls @ref _Thread_Enable_dispatch which in turns calls
 * @ref _Thread_Dispatch.  If the enable dispatch is inlined, then
 * one subroutine call is avoided entirely.
 *
 * Port Specific Information:
 *
 * The v850 is a RISC CPU which typically has enough memory to justify
 * the inlining of this method.
 */
#define CPU_INLINE_ENABLE_DISPATCH       TRUE

/**
 * Should the body of the search loops in _Thread_queue_Enqueue_priority
 * be unrolled one time?  In unrolled each iteration of the loop examines
 * two "nodes" on the chain being searched.  Otherwise, only one node
 * is examined per iteration.
 *
 * If TRUE, then the loops are unrolled.
 * If FALSE, then the loops are not unrolled.
 *
 * The primary factor in making this decision is the cost of disabling
 * and enabling interrupts (_ISR_Flash) versus the cost of rest of the
 * body of the loop.  On some CPUs, the flash is more expensive than
 * one iteration of the loop body.  In this case, it might be desirable
 * to unroll the loop.  It is important to note that on some CPUs, this
 * code is the longest interrupt disable period in RTEMS.  So it is
 * necessary to strike a balance when setting this parameter.
 *
 * Port Specific Information:
 *
 * The v850 is a RISC CPU which typically has enough memory to justify
 * the unrolling of this method.
 */
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/**
 * Does RTEMS manage a dedicated interrupt stack in software?
 *
 * If TRUE, then a stack is allocated in @ref _ISR_Handler_initialization.
 * If FALSE, nothing is done.
 *
 * If the CPU supports a dedicated interrupt stack in hardware,
 * then it is generally the responsibility of the BSP to allocate it
 * and set it up.
 *
 * If the CPU does not support a dedicated interrupt stack, then
 * the porter has two options: (1) execute interrupts on the
 * stack of the interrupted task, and (2) have RTEMS manage a dedicated
 * interrupt stack.
 *
 * If this is TRUE, @ref CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 * Only one of @ref CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 * @ref CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 * possible that both are FALSE for a particular CPU.  Although it
 * is unclear what that would imply about the interrupt processing
 * procedure on that CPU.
 *
 * Port Specific Information:
 *
 * The v850 does not have support for a hardware interrupt stack.
 */
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE

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
 * Does this CPU have hardware support for a dedicated interrupt stack?
 *
 * If TRUE, then it must be installed during initialization.
 * If FALSE, then no installation is performed.
 *
 * If this is TRUE, @ref CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 * Only one of @ref CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 * @ref CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 * possible that both are FALSE for a particular CPU.  Although it
 * is unclear what that would imply about the interrupt processing
 * procedure on that CPU.
 *
 * Port Specific Information:
 *
 * The v850 does not have support for a hardware interrupt stack.
 */
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE

/**
 * Does RTEMS allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 * If TRUE, then the memory is allocated during initialization.
 * If FALSE, then the memory is allocated during initialization.
 *
 * This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_ALLOCATE_INTERRUPT_STACK TRUE

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

/**
 * Does this port provide a CPU dependent IDLE task implementation?
 *
 * If TRUE, then the routine @ref _CPU_Thread_Idle_body
 * must be provided and is the default IDLE thread body instead of
 * @ref _CPU_Thread_Idle_body.
 *
 * If FALSE, then use the generic IDLE thread body if the BSP does
 * not provide one.
 *
 * This is intended to allow for supporting processors which have
 * a low power or idle mode.  When the IDLE thread is executed, then
 * the CPU can be powered down.
 *
 * The order of precedence for selecting the IDLE thread body is:
 *
 *   -#  BSP provided
 *   -#  CPU dependent (if provided)
 *   -#  generic (if no BSP and no CPU dependent)
 *
 * Port Specific Information:
 *
 * There does not appear to be a reason for the v850 port itself to provide
 * a special idle task.
 */
#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

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

/**
 * The following is the variable attribute used to force alignment
 * of critical RTEMS structures.  On some processors it may make
 * sense to have these aligned on tighter boundaries than
 * the minimum requirements of the compiler in order to have as
 * much of the critical data area as possible in a cache line.
 *
 * The placement of this macro in the declaration of the variables
 * is based on the syntactically requirements of the GNU C
 * "__attribute__" extension.  For example with GNU C, use
 * the following to force a structures to a 32 byte boundary.
 *
 *     __attribute__ ((aligned (32)))
 *
 * @note Currently only the Priority Bit Map table uses this feature.
 *       To benefit from using this, the data must be heavily
 *       used so it will stay in the cache and used frequently enough
 *       in the executive to justify turning this on.
 *
 * Port Specific Information:
 *
 * Until proven otherwise, use the compiler default.
 */
#define CPU_STRUCTURE_ALIGNMENT

/**
 * The v850 should use 64-bit timestamps and inline them.
 */
#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

/**
 * @defgroup CPUEndian Processor Dependent Endianness Support
 *
 * This group assists in issues related to processor endianness.
 *
 */
/**@{**/

/**
 * Define what is required to specify how the network to host conversion
 * routines are handled.
 *
 * @note @a CPU_BIG_ENDIAN and @a CPU_LITTLE_ENDIAN should NOT have the
 * same values.
 *
 * @see CPU_LITTLE_ENDIAN
 *
 * Port Specific Information:
 *
 * The v850 is little endian.
 */
#define CPU_BIG_ENDIAN  FALSE

/**
 * Define what is required to specify how the network to host conversion
 * routines are handled.
 *
 * @note @ref CPU_BIG_ENDIAN and @ref CPU_LITTLE_ENDIAN should NOT have the
 * same values.
 *
 * @see CPU_BIG_ENDIAN
 *
 * Port Specific Information:
 *
 * The v850 is little endian.
 */
#define CPU_LITTLE_ENDIAN TRUE

/** @} */

/**
 * @ingroup CPUInterrupt
 * The following defines the number of bits actually used in the
 * interrupt field of the task mode.  How those bits map to the
 * CPU interrupt levels is defined by the routine @ref _CPU_ISR_Set_level.
 *
 * Port Specific Information:
 *
 * The v850 only has a single bit in the CPU for interrupt disable/enable.
 */
#define CPU_MODES_INTERRUPT_MASK   0x00000001

#define CPU_PER_CPU_CONTROL_SIZE 0

typedef struct {
  /* There is no CPU specific per-CPU state */
} CPU_Per_CPU_control;

/**
 * @defgroup CPUContext Processor Dependent Context Management
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
/**@{**/

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

/** @} */

/**
 * @defgroup CPUInterrupt Processor Dependent Interrupt Management
 *
 * On some CPUs, RTEMS supports a software managed interrupt stack.
 * This stack is allocated by the Interrupt Manager and the switch
 * is performed in @ref _ISR_Handler.  These variables contain pointers
 * to the lowest and highest addresses in the chunk of memory allocated
 * for the interrupt stack.  Since it is unknown whether the stack
 * grows up or down (in general), this give the CPU dependent
 * code the option of picking the version it wants to use.
 *
 * @note These two variables are required if the macro
 *       @ref CPU_HAS_SOFTWARE_INTERRUPT_STACK is defined as TRUE.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
/**@{**/

/**
 * @ingroup CPUContext
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

/* XXX this should not be needed on PIC architectures */
/* XXX evaluate removing it */
#if 0
/**
 * This defines the number of entries in the @ref _ISR_Vector_table managed
 * by RTEMS.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS      32
#endif

/**
 * This defines the highest interrupt vector number for this port.
 */
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/**
 * This is defined if the port has a special way to report the ISR nesting
 * level.  Most ports maintain the variable @a _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/** @} */

/**
 * @ingroup CPUContext
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
 * This number corresponds to the byte alignment requirement for memory
 * buffers allocated by the partition manager.  This alignment requirement
 * may be stricter than that for the data types alignment specified by
 * @ref CPU_ALIGNMENT.  It is common for the partition to follow the same
 * alignment requirement as @ref CPU_ALIGNMENT.  If the @ref CPU_ALIGNMENT is
 * strict enough for the partition, then this should be set to
 * @ref CPU_ALIGNMENT.
 *
 * @note  This does not have to be a power of 2.  It does have to
 *        be greater or equal to than @ref CPU_ALIGNMENT.
 *
 * Port Specific Information:
 *
 * There is no apparent reason why this should be larger than CPU_ALIGNMENT.
 */
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

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

/*
 *  ISR handler macros
 */

/**
 * @addtogroup CPUInterrupt
 */
/**@{**/

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
 * @ingroup CPUContext
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
  bool              is_fp
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
 * @ingroup CPUContext
 * The purpose of this macro is to allow the initial pointer into
 * a floating point context area (used to save the floating point
 * context) to be at an arbitrary place in the floating point
 * context area.
 *
 * This is necessary because some FP units are designed to have
 * their context saved as a stack which grows into lower addresses.
 * Other FP units can be saved by simply moving registers into offsets
 * from the base of the context area.  Finally some FP units provide
 * a "dump context" instruction which could fill in from high to low
 * or low to high based on the whim of the CPU designers.
 *
 * @param[in] _base is the lowest physical address of the floating point
 *        context area
 * @param[in] _offset is the offset into the floating point area
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )
#endif

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
#define _CPU_Fatal_halt( _error ) \
  do { \
    __asm__ __volatile__ ( "di" ); \
    __asm__ __volatile__ ( "mov %0, r10; " : "=r" ((_error)) ); \
    __asm__ __volatile__ ( "halt" ); \
  } while (0)

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

/**
 * @defgroup CPUBitfield Processor Dependent Bitfield Manipulation
 *
 * This set of routines are used to implement fast searches for
 * the most important ready task.
 */
/**@{**/

/**
 * This definition is set to TRUE if the port uses the generic bitfield
 * manipulation implementation.
 */
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

/**
 * This definition is set to TRUE if the port uses the data tables provided
 * by the generic bitfield manipulation implementation.
 * This can occur when actually using the generic bitfield manipulation
 * implementation or when implementing the same algorithm in assembly
 * language for improved performance.  It is unlikely that a port will use
 * the data if it has a bitfield scan instruction.
 *
 * Port Specific Information:
 *
 * There is no single v850 instruction to do a bit scan so there is
 * no CPU specific implementation of bit field scanning. The empty
 * stub routines are left as a place holder in case someone figures
 * out how to do a v850 implementation better than the generic algorithm.
 */
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

/**
 * This routine sets @a _output to the bit number of the first bit
 * set in @a _value.  @a _value is of CPU dependent type
 * @a Priority_bit_map_Control.  This type may be either 16 or 32 bits
 * wide although only the 16 least significant bits will be used.
 *
 * There are a number of variables in using a "find first bit" type
 * instruction.
 *
 *   -# What happens when run on a value of zero?
 *   -# Bits may be numbered from MSB to LSB or vice-versa.
 *   -# The numbering may be zero or one based.
 *   -# The "find first bit" instruction may search from MSB or LSB.
 *
 * RTEMS guarantees that (1) will never happen so it is not a concern.
 * (2),(3), (4) are handled by the macros @ref _CPU_Priority_Mask and
 * @ref _CPU_Priority_bits_index.  These three form a set of routines
 * which must logically operate together.  Bits in the _value are
 * set and cleared based on masks built by @ref _CPU_Priority_Mask.
 * The basic major and minor values calculated by @ref _Priority_Major
 * and @ref _Priority_Minor are "massaged" by @ref _CPU_Priority_bits_index
 * to properly range between the values returned by the "find first bit"
 * instruction.  This makes it possible for @ref _Priority_Get_highest to
 * calculate the major and directly index into the minor table.
 * This mapping is necessary to ensure that 0 (a high priority major/minor)
 * is the first bit found.
 *
 * This entire "find first bit" and mapping process depends heavily
 * on the manner in which a priority is broken into a major and minor
 * components with the major being the 4 MSB of a priority and minor
 * the 4 LSB.  Thus (0 << 4) + 0 corresponds to priority 0 -- the highest
 * priority.  And (15 << 4) + 14 corresponds to priority 254 -- the next
 * to the lowest priority.
 *
 * If your CPU does not have a "find first bit" instruction, then
 * there are ways to make do without it.  Here are a handful of ways
 * to implement this in software:
 *
@verbatim
      - a series of 16 bit test instructions
      - a "binary search using if's"
      - _number = 0
        if _value > 0x00ff
          _value >>=8
          _number = 8;

        if _value > 0x0000f
          _value >=8
          _number += 4

        _number += bit_set_table[ _value ]
@endverbatim

 *   where bit_set_table[ 16 ] has values which indicate the first
 *     bit set
 *
 * @param[in] _value is the value to be scanned
 * @param[in] _output is the first bit set
 *
 * Port Specific Information:
 *
 * There is no single v850 instruction to do a bit scan so there is
 * no CPU specific implementation of bit field scanning.
 */
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    (_output) = 0;   /* do something to prevent warnings */ \
  }
#endif

/* end of Bitfield handler macros */

/**
 * This routine builds the mask which corresponds to the bit fields
 * as searched by @ref _CPU_Bitfield_Find_first_bit.  See the discussion
 * for that routine.
 *
 * Port Specific Information:
 *
 * There is no single v850 instruction to do a bit scan so there is
 * no CPU specific implementation of bit field scanning.
 */
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

#endif

/**
 * This routine translates the bit numbers returned by
 * @ref _CPU_Bitfield_Find_first_bit into something suitable for use as
 * a major or minor component of a priority.  See the discussion
 * for that routine.
 *
 * @param[in] _priority is the major or minor number to translate
 *
 * Port Specific Information:
 *
 * There is no single v850 instruction to do a bit scan so there is
 * no CPU specific implementation of bit field scanning.
 */
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

#endif

/* end of Priority handler macros */

/** @} */

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

/**
 * @addtogroup CPUContext
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
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

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

static inline void _CPU_Context_volatile_clobber( uintptr_t pattern )
{
  /* TODO */
}

static inline void _CPU_Context_validate( uintptr_t pattern )
{
  while (1) {
    /* TODO */
  }
}

/** @} */

/* FIXME */
typedef CPU_Interrupt_frame CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

/**
 * @ingroup CPUEndian
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
 * @ingroup CPUEndian
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

#ifdef __cplusplus
}
#endif

#endif
