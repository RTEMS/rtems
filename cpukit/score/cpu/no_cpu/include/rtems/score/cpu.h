/**
 * @file
 *
 * @brief NO_CPU Department Source
 *
 * This include file contains information pertaining to the NO_CPU
 * processor.
 */

/*
 *  This include file contains information pertaining to the XXX
 *  processor.
 *
 *  @note This file is part of a porting template that is intended
 *  to be used as the starting point when porting RTEMS to a new
 *  CPU family.  The following needs to be done when using this as
 *  the starting point for a new port:
 *
 *  + Anywhere there is an XXX, it should be replaced
 *    with information about the CPU family being ported to.
 *
 *  + At the end of each comment section, there is a heading which
 *    says "Port Specific Information:".  When porting to RTEMS,
 *    add CPU family specific information in this section
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/no_cpu.h>

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
 * XXX document implementation including references if appropriate
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/**
 * Does the RTEMS invoke the user's ISR with the vector number and
 * a pointer to the saved interrupt frame (1) or just the vector
 * number (0)?
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_ISR_PASSES_FRAME_POINTER FALSE

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
 * The macro name "NO_CPU_HAS_FPU" should be made CPU specific.
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
 * XXX document implementation including references if appropriate
 */
#if ( NO_CPU_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif
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
 * XXX document implementation including references if appropriate
 */
#define CPU_ALL_TASKS_ARE_FP     TRUE

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
 * XXX document implementation including references if appropriate
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
 * XXX document implementation including references if appropriate
 */
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

/**
 * @brief Enables a robust thread dispatch if set to TRUE.
 *
 * In general, it is an application bug to call blocking operating system
 * services with interrupts disabled.  In most situations this only increases
 * the interrupt latency.  However, on SMP configurations or on some CPU port
 * like ARM Cortex-M it leads to undefined system behaviour.  It order to ease
 * the application development, this error condition is checked at run-time in
 * case this CPU port option is defined to TRUE.
 */
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
 * XXX document implementation including references if appropriate
 */
#define CPU_STACK_GROWS_UP               TRUE

/**
 * The maximum cache line size in bytes.
 *
 * The actual processor may use no cache or a smaller cache line size.
 */
#define CPU_CACHE_LINE_BYTES 32

/**
 * The following is the variable attribute used to force alignment
 * of critical RTEMS structures.  On some processors it may make
 * sense to have these aligned on tighter boundaries than
 * the minimum requirements of the compiler in order to have as
 * much of the critical data area as possible in a cache line.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * The following defines the number of bits actually used in the
 * interrupt field of the task mode.  How those bits map to the
 * CPU interrupt levels is defined by the routine @ref _CPU_ISR_Set_level.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_MODES_INTERRUPT_MASK   0x00000001

/**
 * @brief Maximum number of processors of all systems supported by this CPU
 * port.
 */
#define CPU_MAXIMUM_PROCESSORS 32

/*
 *  Processor defined structures required for cpukit/score.
 *
 *  Port Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

/* may need to put some structures here.  */

/**
 * @defgroup RTEMSScoreCPUExampleContext Processor Dependent Context Management
 * 
 * @ingroup RTEMSScoreCPUExample
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
 * XXX document implementation including references if appropriate
 * 
 */
/** @{ **/
/** @} */

/**
 * @defgroup Management Management
 *
 * @ingroup RTEMSScoreCPUExample
 *
 * @brief Management
 */

/**
 * @addtogroup Management
 * This defines the minimal set of integer and processor state registers
 * that must be saved during a voluntary context switch from one thread
 * to another.
 */
typedef struct {
    /**
     * This field is a hint that a port will have a number of integer
     * registers that need to be saved at a context switch.
     */
    uint32_t   some_integer_register;
    /**
     * This field is a hint that a port will have a number of system
     * registers that need to be saved at a context switch.
     */
    uint32_t   some_system_register;

    /**
     * This field is a hint that a port will have a register that
     * is the stack pointer.
     */
    uint32_t   stack_pointer;

#ifdef RTEMS_SMP
    /**
     * @brief On SMP configurations the thread context must contain a boolean
     * indicator to signal if this context is executing on a processor.
     *
     * This field must be updated during a context switch.  The context switch
     * to the heir must wait until the heir context indicates that it is no
     * longer executing on a processor.  This indicator must be updated using
     * an atomic test and set operation to ensure that at most one processor
     * uses the heir context at the same time.  The context switch must also
     * check for a potential new heir thread for this processor in case the
     * heir context is not immediately available.  Update the executing thread
     * for this processor only if necessary to avoid a cache line
     * monopolization.
     *
     * @code
     * void _CPU_Context_switch(
     *   Context_Control *executing_context,
     *   Context_Control *heir_context
     * )
     * {
     *   save( executing_context );
     *
     *   executing_context->is_executing = false;
     *   memory_barrier();
     *
     *   if ( test_and_set( &heir_context->is_executing ) ) {
     *     do {
     *       Per_CPU_Control *cpu_self = _Per_CPU_Get_snapshot();
     *       Thread_Control *executing = cpu_self->executing;
     *       Thread_Control *heir = cpu_self->heir;
     *
     *       if ( heir != executing ) {
     *         cpu_self->executing = heir;
     *         heir_context = (Context_Control *)
     *           ((uintptr_t) heir + (uintptr_t) executing_context
     *             - (uintptr_t) executing)
     *       }
     *     } while ( test_and_set( &heir_context->is_executing ) );
     *   }
     *
     *   restore( heir_context );
     * }
     * @endcode
     */
    volatile bool is_executing;
#endif
} Context_Control;

/**
 * @addtogroup Management
 *
 * This macro returns the stack pointer associated with @a _context.
 *
 * @param[in] _context is the thread context area to access
 *
 * @return This method returns the stack pointer.
 */
#define _CPU_Context_Get_SP( _context ) \
  (_context)->stack_pointer

/**
 * @addtogroup Management
 * 
 * This defines the complete set of floating point registers that must
 * be saved during any context switch from one thread to another.
 */
typedef struct {
    /** FPU registers are listed here */
    double      some_float_register;
} Context_Control_fp;

/**
 * @addtogroup Management
 * 
 * This defines the set of integer and processor state registers that must
 * be saved during an interrupt.  This set does not include any which are
 * in @ref Context_Control.
 */
typedef struct {
    /** 
     * This field is a hint that a port will have a number of integer
     * registers that need to be saved when an interrupt occurs or
     * when a context switch occurs at the end of an ISR.
     */
    uint32_t   special_interrupt_register;
} CPU_Interrupt_frame;

/**
 * This variable is optional.  It is used on CPUs on which it is difficult
 * to generate an "uninitialized" FP context.  It is filled in by
 * @ref _CPU_Initialize and copied into the task's FP context area during
 * @ref _CPU_Context_Initialize.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
extern Context_Control_fp _CPU_Null_fp_context;

/**
 * @defgroup RTEMSScoreCPUExampleInterrupt Processor Dependent Interrupt Management
 * 
 * @ingroup RTEMSScoreCPUExample
 *
 * RTEMS supports a software managed interrupt stack.  The interrupt stacks
 * are statically allocated by <rtems/confdefs.h> and the switch is performed
 * by hardware or the interrupt processing code.  These variables contain
 * pointers to the lowest and highest addresses in the chunk of memory
 * allocated for the interrupt stack.  Since it is unknown whether the stack
 * grows up or down (in general), this gives the CPU dependent code the option
 * of picking the version it wants to use.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 *
 *  Port Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

/* XXX: if needed, put more variables here */

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
 * The size of the floating point context area.  On some CPUs this
 * will not be a "sizeof" because the format of the floating point
 * area is not defined -- only the size is.  This is usually on
 * CPUs with a "floating point save context" instruction.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/**
 * Amount of extra stack (above minimum stack size) required by
 * MPCI receive server thread.  Remember that in a multiprocessor
 * system this thread must exist and be able to process all directives.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This defines the number of entries in the _ISR_Vector_table managed by RTEMS
 * in case CPU_SIMPLE_VECTORED_INTERRUPTS is defined to TRUE.  It must be a
 * compile-time constant.
 *
 * It must be undefined in case CPU_SIMPLE_VECTORED_INTERRUPTS is defined to
 * FALSE.
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS      32

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This defines the highest interrupt vector number for this port in case
 * CPU_SIMPLE_VECTORED_INTERRUPTS is defined to TRUE.  It must be less than
 * CPU_INTERRUPT_NUMBER_OF_VECTORS.  It may be not a compile-time constant.
 *
 * It must be undefined in case CPU_SIMPLE_VECTORED_INTERRUPTS is defined to
 * FALSE.
 */
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This is defined if the port has a special way to report the ISR nesting
 * level.  Most ports maintain the variable @a _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
 * Should be large enough to run all RTEMS tests.  This ensures
 * that a "reasonable" small application should not have any problems.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_STACK_MINIMUM_SIZE          (1024*4)

/**
 * Size of a pointer.
 *
 * This must be an integer literal that can be used by the assembler.  This
 * value will be used to calculate offsets of structure members.  These
 * offsets will be used in assembler code.
 */
#define CPU_SIZEOF_POINTER         4

/**
 * CPU's worst alignment requirement for data types on a byte boundary.  This
 * alignment does not take into account the requirements for the stack.  It
 * must be a power of two greater than or equal to two.  The power of two
 * requirement makes it possible to align values easily using simple bit
 * operations.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
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
 * NOTE:  It must be a power of two greater than or equal to two.  The
 *        requirement to be a multiple of two is because the heap uses the
 *        least significant field of the front and back flags to indicate that
 *        a block is in use or free.  So you do not want any odd length blocks
 *        really putting length data in that bit.
 *
 *        On byte oriented architectures, @ref CPU_HEAP_ALIGNMENT normally will
 *        have to be greater or equal to than @ref CPU_ALIGNMENT to ensure that
 *        elements allocated from the heap meet all restrictions.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

/**
 * This number corresponds to the byte alignment requirement for the
 * stack.  This alignment requirement may be stricter than that for the
 * data types alignment specified by @ref CPU_ALIGNMENT.  If the
 * @ref CPU_ALIGNMENT is strict enough for the stack, then this should be
 * set to 0.
 *
 * NOTE: This must be a power of 2 either 0 or greater than @ref CPU_ALIGNMENT.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_STACK_ALIGNMENT        0

/**
 * The alignment of the interrupt stack in bytes.
 *
 * The alignment should take the stack ABI and the cache line size into
 * account.
 */
#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  ISR handler macros
 */

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * Support routine to initialize the RTEMS vector table after it is allocated.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Initialize_vectors()

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * Disable all interrupts for an RTEMS critical section.  The previous
 * level is returned in @a _isr_cookie.
 *
 * @param[out] _isr_cookie will contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Disable( _isr_cookie ) \
  { \
    (_isr_cookie) = 0;   /* do something to prevent warnings */ \
  }

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 * This indicates the end of an RTEMS critical section.  The parameter
 * @a _isr_cookie is not modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
  }

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This temporarily restores the interrupt to @a _isr_cookie before immediately
 * disabling them again.  This is used to divide long RTEMS critical
 * sections into two or more parts.  The parameter @a _isr_cookie is not
 * modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Flash( _isr_cookie ) \
  { \
  }

/**
 * @brief Returns true if interrupts are enabled in the specified ISR level,
 * otherwise returns false.
 *
 * @param[in] level The ISR level.
 *
 * @retval true Interrupts are enabled in the ISR level.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return false;
}

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 *
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
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Set_level( new_level ) \
  { \
  }

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * Return the current interrupt disable level for this task in
 * the format used by the interrupt level portion of the task mode.
 *
 * NOTE: This routine usually must be implemented as a subroutine.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
uint32_t   _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/* Context handler macros */

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 *
 * @brief Destroys the context of the thread.
 *
 * It must be implemented as a macro and an implementation is optional.  The
 * default implementation does nothing.
 *
 * @param[in] _the_thread The corresponding thread.
 * @param[in] _the_context The context to destroy.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Destroy( _the_thread, _the_context ) \
  { \
  }

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
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
 * The ISR dispatch disable field of the context must be cleared to zero if it
 * is used by the CPU port.  Otherwise, a thread restart results in
 * unpredictable behaviour.
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
 * @param[in] _tls_area The thread-local storage (TLS) area.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp, _tls_area ) \
  { \
  }

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
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

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
   *(*(_destination)) = _CPU_Null_fp_context; \
  }

/* end of Context handler macros */

/* Fatal Error manager macros */

/**
 * This routine copies _error into a known place -- typically a stack
 * location or a register, optionally disables interrupts, and
 * halts/stops the CPU.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Fatal_halt( _source, _error ) \
  { \
  }

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

/**
 * @defgroup RTEMSScoreCPUExampleBitfield Processor Dependent Bitfield Manipulation
 * 
 * @ingroup RTEMSScoreCPUExample
 *
 * This set of routines are used to implement fast searches for
 * the most important ready task.
 * 
 */
/**@{**/

/**
 * This definition is set to TRUE if the port uses the generic bitfield
 * manipulation implementation.
 */
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

/**
 * This routine sets @a _output to the bit number of the first bit
 * set in @a _value.  @a _value is of CPU dependent type
 * @a Priority_bit_map_Word.  This type may be either 16 or 32 bits
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
 * XXX document implementation including references if appropriate
 */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    (_output) = 0;   /* do something to prevent warnings */ \
  }
#endif

/** @} */

/* end of Bitfield handler macros */

/**
 * This routine builds the mask which corresponds to the bit fields
 * as searched by @ref _CPU_Bitfield_Find_first_bit.  See the discussion
 * for that routine.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

#endif

/**
 * @addtogroup RTEMSScoreCPUExampleBitfield
 * 
 * This routine translates the bit numbers returned by
 * @ref _CPU_Bitfield_Find_first_bit into something suitable for use as
 * a major or minor component of a priority.  See the discussion
 * for that routine.
 *
 * @param[in] _priority is the major or minor number to translate
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

#endif

/* end of Priority handler macros */

/* functions */

/**
 * This routine performs CPU dependent initialization.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Initialize(void);

typedef void ( *CPU_ISR_raw_handler )( void );

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This routine installs a "raw" interrupt handler directly into the
 * processor's vector table.
 *
 * This routine is not used by architecture-independent code and thus optional.
 *
 * @param[in] vector is the vector number
 * @param[in] new_handler is the raw ISR handler to install
 * @param[in] old_handler is the previously installed ISR Handler
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
);

typedef void ( *CPU_ISR_handler )( uint32_t );

/**
 * @addtogroup RTEMSScoreCPUExampleInterrupt
 * 
 * This routine installs an interrupt vector.
 *
 * This routine is only used by architecture-independent code if
 * CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE, otherwise it is optional.
 *
 * @param[in] vector is the vector number
 * @param[in] new_handler is the RTEMS ISR handler to install
 * @param[in] old_handler is the previously installed ISR Handler
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

/**
 * This routine is the CPU dependent IDLE thread body.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void *_CPU_Thread_Idle_body( uintptr_t ignored );

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
 * This routine switches from the run context to the heir context.
 *
 * @param[in] run points to the context of the currently executing task
 * @param[in] heir points to the context of the heir task
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
 * This routine is generally used only to restart self in an
 * efficient manner.  It may simply be a label in @ref _CPU_Context_switch.
 *
 * @param[in] new_context points to the context to be restored.
 *
 * NOTE: May be unnecessary to reload some registers.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_NO_RETURN;

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
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

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 * 
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
  uint32_t processor_state_register;
  uint32_t integer_registers [1];
  double float_registers [1];
} CPU_Exception_frame;

/**
 * @brief Prints the exception frame via printk().
 *
 * @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
 */
void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

/**
 * @defgroup RTEMSScoreCPUExampleCPUEndian CPUEndian
 * 
 * @ingroup RTEMSScoreCPUExample
 * 
 * @brief CPUEndian
 *
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
 * XXX document implementation including references if appropriate
 */
static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return swapped;
}

/**
 * This routine swaps a 16 bir quantity.
 *
 * @param[in] value is the value to be swapped
 * @return the value after being endian swapped
 */
#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))
  
/** @} */

/**
 * @brief Unsigned integer type for CPU counter values.
 */
typedef uint32_t CPU_Counter_ticks;

/**
 * @brief Returns the current CPU counter frequency in Hz.
 *
 * @return The current CPU counter frequency in Hz.
 */
uint32_t _CPU_Counter_frequency( void );

/**
 * @brief Returns the current CPU counter value.
 *
 * A CPU counter is some free-running counter.  It ticks usually with a
 * frequency close to the CPU or system bus clock.  The board support package
 * must ensure that this function works before the RTEMS initialization.
 * Otherwise invalid profiling statistics will be gathered.
 *
 * @return The current CPU counter value.
 */
CPU_Counter_ticks _CPU_Counter_read( void );

/**
 * @brief Returns the difference between the second and first CPU counter
 * value.
 *
 * This operation may be carried out as a modulo operation depending on the
 * range of the CPU counter device.
 *
 * @param[in] second The second CPU counter value.
 * @param[in] first The first CPU counter value.
 *
 * @return Returns second minus first modulo counter period.
 */
static inline CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

#ifdef RTEMS_SMP
  /**
   * @brief Performs CPU specific SMP initialization in the context of the boot
   * processor.
   *
   * This function is invoked on the boot processor during system
   * initialization.  All interrupt stacks are allocated at this point in case
   * the CPU port allocates the interrupt stacks.  This function is called
   * before _CPU_SMP_Start_processor() or _CPU_SMP_Finalize_initialization() is
   * used.
   *
   * @return The count of physically or virtually available processors.
   * Depending on the configuration the application may use not all processors.
   */
  uint32_t _CPU_SMP_Initialize( void );

  /**
   * @brief Starts a processor specified by its index.
   *
   * This function is invoked on the boot processor during system
   * initialization.
   *
   * This function will be called after _CPU_SMP_Initialize().
   *
   * @param[in] cpu_index The processor index.
   *
   * @retval true Successful operation.
   * @retval false Unable to start this processor.
   */
  bool _CPU_SMP_Start_processor( uint32_t cpu_index );

  /**
   * @brief Performs final steps of CPU specific SMP initialization in the
   * context of the boot processor.
   *
   * This function is invoked on the boot processor during system
   * initialization.
   *
   * This function will be called after all processors requested by the
   * application have been started.
   *
   * @param[in] cpu_count The minimum value of the count of processors
   * requested by the application configuration and the count of physically or
   * virtually available processors.
   */
  void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

  /**
   * @brief Prepares a CPU to start multitasking in terms of SMP.
   *
   * This function is invoked on all processors requested by the application
   * during system initialization.
   *
   * This function will be called after all processors requested by the
   * application have been started right before the context switch to the first
   * thread takes place.
   */
  void _CPU_SMP_Prepare_start_multitasking( void );

  /**
   * @brief Returns the index of the current processor.
   *
   * An architecture specific method must be used to obtain the index of the
   * current processor in the system.  The set of processor indices is the
   * range of integers starting with zero up to the processor count minus one.
   */
  static inline uint32_t _CPU_SMP_Get_current_processor( void )
  {
    return 123;
  }

  /**
   * @brief Sends an inter-processor interrupt to the specified target
   * processor.
   *
   * This operation is undefined for target processor indices out of range.
   *
   * @param[in] target_processor_index The target processor index.
   */
  void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

  /**
   * @brief Broadcasts a processor event.
   *
   * Some architectures provide a low-level synchronization primitive for
   * processors in a multi-processor environment.  Processors waiting for this
   * event may go into a low-power state and stop generating system bus
   * transactions.  This function must ensure that preceding store operations
   * can be observed by other processors.
   *
   * @see _CPU_SMP_Processor_event_receive().
   */
  static inline void _CPU_SMP_Processor_event_broadcast( void )
  {
    __asm__ volatile ( "" : : : "memory" );
  }

  /**
   * @brief Receives a processor event.
   *
   * This function will wait for the processor event and may wait forever if no
   * such event arrives.
   *
   * @see _CPU_SMP_Processor_event_broadcast().
   */
  static inline void _CPU_SMP_Processor_event_receive( void )
  {
    __asm__ volatile ( "" : : : "memory" );
  }

  /**
   * @brief Gets the is executing indicator of the thread context.
   *
   * @param[in] context The context.
   */
  static inline bool _CPU_Context_Get_is_executing(
    const Context_Control *context
  )
  {
    return context->is_executing;
  }

  /**
   * @brief Sets the is executing indicator of the thread context.
   *
   * @param[in] context The context.
   * @param[in] is_executing The new value for the is executing indicator.
   */
  static inline void _CPU_Context_Set_is_executing(
    Context_Control *context,
    bool is_executing
  )
  {
    context->is_executing = is_executing;
  }

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif

#ifdef __cplusplus
}
#endif

#endif
