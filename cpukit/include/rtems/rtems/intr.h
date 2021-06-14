/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Interrupt Manager API.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/intr/if/header */

#ifndef _RTEMS_RTEMS_INTR_H
#define _RTEMS_RTEMS_INTR_H

#include <rtems/rtems/status.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/smplock.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/intr/if/group */

/**
 * @defgroup RTEMSAPIClassicIntr Interrupt Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief Any real-time executive must provide a mechanism for quick response
 *   to externally generated interrupts to satisfy the critical time
 *   constraints of the application.  The Interrupt Manager provides this
 *   mechanism for RTEMS. This manager permits quick interrupt response times
 *   by providing the critical ability to alter task execution which allows a
 *   task to be preempted upon exit from an ISR.
 */

/* Generated from spec:/rtems/intr/if/isr */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief This type defines the return type of interrupt service routines.
 *
 * This type can be used to document interrupt service routines in the source
 * code.
 */
typedef ISR_Handler rtems_isr;

/* Generated from spec:/rtems/intr/if/isr-entry */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Interrupt service routines installed by rtems_interrupt_catch() shall
 *   have this function pointer type.
 */
#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
  typedef ISR_Handler_entry rtems_isr_entry;
#else
  typedef void ( *rtems_isr_entry )( void * );
#endif

/* Generated from spec:/rtems/intr/if/level */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief This integer type represents interrupt levels.
 */
typedef ISR_Level rtems_interrupt_level;

/* Generated from spec:/rtems/intr/if/lock */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief This structure represents an ISR lock.
 */
typedef ISR_lock_Control rtems_interrupt_lock;

/* Generated from spec:/rtems/intr/if/lock-context */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief This structure provides an ISR lock context for acquire and release
 *   pairs.
 */
typedef ISR_lock_Context rtems_interrupt_lock_context;

/* Generated from spec:/rtems/intr/if/vector-number */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief This integer type represents interrupt vector numbers.
 */
typedef ISR_Vector_number rtems_vector_number;

/* Generated from spec:/rtems/intr/if/catch */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Establishes an interrupt service routine.
 *
 * @param new_isr_handler is the new interrupt service routine.
 *
 * @param vector is the interrupt vector number.
 *
 * @param[out] old_isr_handler is the pointer to an ::rtems_isr_entry object.
 *   When the directive call is successful, the previous interrupt service
 *   routine established for this interrupt vector will be stored in this
 *   object.
 *
 * This directive establishes an interrupt service routine (ISR) for the
 * interrupt specified by the ``vector`` number.  The ``new_isr_handler``
 * parameter specifies the entry point of the ISR.  The entry point of the
 * previous ISR for the specified vector is returned in ``old_isr_handler``.
 *
 * To release an interrupt vector, pass the old handler's address obtained when
 * the vector was first capture.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The interrupt vector number was illegal.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``new_isr_handler`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``old_isr_handler`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive is only available where the target architecture support
 *   enabled simple vectored interrupts.
 * @endparblock
 */
rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry     new_isr_handler,
  rtems_vector_number vector,
  rtems_isr_entry    *old_isr_handler
);

#if !defined(RTEMS_SMP)
  /* Generated from spec:/rtems/intr/if/disable */

  /**
   * @ingroup RTEMSAPIClassicIntr
   *
   * @brief Disables the maskable interrupts on the current processor.
   *
   * @param[out] _isr_cookie is a variable of type ::rtems_interrupt_level which
   *   will be used to save the previous interrupt level.
   *
   * This directive disables all maskable interrupts on the current processor and
   * returns the previous interrupt level in ``_isr_cookie``.
   *
   * @par Notes
   * @parblock
   * A later invocation of the rtems_interrupt_enable() directive should be used
   * to restore the previous interrupt level.
   *
   * This directive is implemented as a macro which sets the ``_isr_cookie``
   * parameter.
   *
   * @code
   * #include <rtems.h>
   *
   * void local_critical_section( void )
   * {
   *   rtems_interrupt_level level;
   *
   *   // Please note that the rtems_interrupt_disable() is a macro.  The
   *   // previous interrupt level (before the maskable interrupts are
   *   // disabled) is returned here in the level macro parameter.  This
   *   // would be wrong:
   *   //
   *   // rtems_interrupt_disable( &level );
   *   rtems_interrupt_disable( level );
   *
   *   // Here is the critical section: maskable interrupts are disabled
   *
   *   {
   *     rtems_interrupt_level nested_level;
   *
   *     rtems_interrupt_disable( nested_level );
   *
   *     // Here is a nested critical section
   *
   *     rtems_interrupt_enable( nested_level );
   *   }
   *
   *   // Maskable interrupts are still disabled
   *
   *   rtems_interrupt_enable( level );
   * }
   * @endcode
   * @endparblock
   *
   * @par Constraints
   * @parblock
   * The following constraints apply to this directive:
   *
   * * The directive may be called from within any runtime context.
   *
   * * The directive will not cause the calling task to be preempted.
   *
   * * Where the system was built with SMP support enabled, the directive is not
   *   available.  Its use will result in compiler warnings and linker errors.
   *   The rtems_interrupt_local_disable() and rtems_interrupt_local_enable()
   *   directives are available in all build configurations.
   * @endparblock
   */
  #define rtems_interrupt_disable( _isr_cookie ) _ISR_Local_disable( _isr_cookie )
#endif

#if !defined(RTEMS_SMP)
  /* Generated from spec:/rtems/intr/if/enable */

  /**
   * @ingroup RTEMSAPIClassicIntr
   *
   * @brief Restores the previous interrupt level on the current processor.
   *
   * @param[in] _isr_cookie is the previous interrupt level to restore.  The
   *   value must be obtained by a previous call to rtems_interrupt_disable() or
   *   rtems_interrupt_flash().
   *
   * This directive restores the interrupt level specified by ``_isr_cookie`` on
   * the current processor.
   *
   * @par Notes
   * @parblock
   * The ``_isr_cookie`` parameter value must be obtained by a previous call to
   * rtems_interrupt_disable() or rtems_interrupt_flash().  Using an otherwise
   * obtained value is undefined behaviour.
   *
   * This directive is unsuitable to enable particular interrupt sources, for
   * example in an interrupt controller.
   * @endparblock
   *
   * @par Constraints
   * @parblock
   * The following constraints apply to this directive:
   *
   * * The directive may be called from within any runtime context.
   *
   * * The directive will not cause the calling task to be preempted.
   *
   * * While at least one maskable interrupt is pending, when the directive
   *   enables maskable interrupts, the pending interrupts are immediately
   *   serviced.  The interrupt service routines may unblock higher priority
   *   tasks which may preempt the calling task.
   *
   * * Where the system was built with SMP support enabled, the directive is not
   *   available.  Its use will result in compiler warnings and linker errors.
   *   The rtems_interrupt_local_disable() and rtems_interrupt_local_enable()
   *   directives are available in all build configurations.
   * @endparblock
   */
  #define rtems_interrupt_enable( _isr_cookie ) _ISR_Local_enable( _isr_cookie )
#endif

#if !defined(RTEMS_SMP)
  /* Generated from spec:/rtems/intr/if/flash */

  /**
   * @ingroup RTEMSAPIClassicIntr
   *
   * @brief Flashes interrupts on the current processor.
   *
   * @param[in,out] _isr_cookie is the previous interrupt level.
   *
   * This directive is functionally equivalent to a calling
   * rtems_interrupt_enable() immediately followed by a
   * rtems_interrupt_disable().  On some architectures it is possible to provide
   * an optimized implementation for this sequence.
   *
   * @par Notes
   * @parblock
   * The ``_isr_cookie`` parameter value must be obtained by a previous call to
   * rtems_interrupt_disable() or rtems_interrupt_flash().  Using an otherwise
   * obtained value is undefined behaviour.
   *
   * Historically, the interrupt flash directive was heavily used in the
   * operating system implementation.  However, this is no longer the case.  The
   * interrupt flash directive is provided for backward compatibility reasons.
   * @endparblock
   *
   * @par Constraints
   * @parblock
   * The following constraints apply to this directive:
   *
   * * The directive may be called from within any runtime context.
   *
   * * The directive will not cause the calling task to be preempted.
   *
   * * Where the system was built with SMP support enabled, the directive is not
   *   available.  Its use will result in compiler warnings and linker errors.
   *   The rtems_interrupt_local_disable() and rtems_interrupt_local_enable()
   *   directives are available in all build configurations.
   * @endparblock
   */
  #define rtems_interrupt_flash( _isr_cookie ) _ISR_Local_flash( _isr_cookie )
#endif

/* Generated from spec:/rtems/intr/if/local-disable */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Disables the maskable interrupts on the current processor.
 *
 * @param[out] _isr_cookie is a variable of type ::rtems_interrupt_level which
 *   will be used to save the previous interrupt level.
 *
 * This directive disables all maskable interrupts on the current processor and
 * returns the previous interrupt level in ``_isr_cookie``.
 *
 * @par Notes
 * @parblock
 * A later invocation of the rtems_interrupt_local_enable() directive should be
 * used to restore the previous interrupt level.
 *
 * This directive is implemented as a macro which sets the ``_isr_cookie``
 * parameter.
 *
 * Where the system was built with SMP support enabled, this will not ensure
 * system wide mutual exclusion.  Use interrupt locks instead, see
 * rtems_interrupt_lock_acquire().  Interrupt disabled critical sections may be
 * used to access processor-specific data structures or disable thread
 * dispatching.
 *
 * @code
 * #include <rtems.h>
 *
 * void local_critical_section( void )
 * {
 *   rtems_interrupt_level level;
 *
 *   // Please note that the rtems_interrupt_local_disable() is a macro.
 *   // The previous interrupt level (before the maskable interrupts are
 *   // disabled) is returned here in the level macro parameter.  This would
 *   // be wrong:
 *   //
 *   // rtems_interrupt_local_disable( &level );
 *   rtems_interrupt_local_disable( level );
 *
 *   // Here is the critical section: maskable interrupts are disabled
 *
 *   {
 *     rtems_interrupt_level nested_level;
 *
 *     rtems_interrupt_local_disable( nested_level );
 *
 *     // Here is a nested critical section
 *
 *     rtems_interrupt_local_enable( nested_level );
 *   }
 *
 *   // Maskable interrupts are still disabled
 *
 *   rtems_interrupt_local_enable( level );
 * }
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define rtems_interrupt_local_disable( _isr_cookie ) \
  _ISR_Local_disable( _isr_cookie )

/* Generated from spec:/rtems/intr/if/local-enable */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Restores the previous interrupt level on the current processor.
 *
 * @param[in] _isr_cookie is the previous interrupt level to restore.  The
 *   value must be obtained by a previous call to
 *   rtems_interrupt_local_disable().
 *
 * This directive restores the interrupt level specified by ``_isr_cookie`` on
 * the current processor.
 *
 * @par Notes
 * @parblock
 * The ``_isr_cookie`` parameter value must be obtained by a previous call to
 * rtems_interrupt_local_disable().  Using an otherwise obtained value is
 * undefined behaviour.
 *
 * This directive is unsuitable to enable particular interrupt sources, for
 * example in an interrupt controller.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * While at least one maskable interrupt is pending, when the directive
 *   enables maskable interrupts, the pending interrupts are immediately
 *   serviced.  The interrupt service routines may unblock higher priority
 *   tasks which may preempt the calling task.
 * @endparblock
 */
#define rtems_interrupt_local_enable( _isr_cookie ) \
  _ISR_Local_enable( _isr_cookie )

/* Generated from spec:/rtems/intr/if/is-in-progress */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Checks if an ISR is in progress on the current processor.
 *
 * This directive returns ``true``, if the current processor is currently
 * servicing an interrupt, and ``false`` otherwise.  A return value of ``true``
 * indicates that the caller is an interrupt service routine, **not** a task.
 * The directives available to an interrupt service routine are restricted.
 *
 * @return Returns true, if the current processor is currently servicing an
 *   interrupt, otherwise false.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define rtems_interrupt_is_in_progress() _ISR_Is_in_progress()

/* Generated from spec:/rtems/intr/if/cause */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Causes the interrupt.
 *
 * @param _vector is the vector number of the interrupt to cause.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive is not implemented.
 * @endparblock
 */
#define rtems_interrupt_cause( _vector ) do { } while ( 0 )

/* Generated from spec:/rtems/intr/if/clear */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Clears the interrupt.
 *
 * @param _vector is the vector number of the interrupt to clear.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive is not implemented.
 * @endparblock
 */
#define rtems_interrupt_clear( _vector ) do { } while ( 0 )

/* Generated from spec:/rtems/intr/if/lock-initialize */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Initializes the ISR lock.
 *
 * @param[out] _lock is the ISR lock to initialize.
 *
 * @param _name is the ISR lock name.  It shall be a string.  The name is only
 *   used where the system was built with profiling support enabled.
 *
 * @par Notes
 * ISR locks may also be statically defined by RTEMS_INTERRUPT_LOCK_DEFINE() or
 * statically initialized by RTEMS_INTERRUPT_LOCK_INITIALIZER().
 */
#define rtems_interrupt_lock_initialize( _lock, _name ) \
  _ISR_lock_Initialize( _lock, _name )

/* Generated from spec:/rtems/intr/if/lock-destroy */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Destroys the ISR lock.
 *
 * @param[in,out] _lock is the ISR lock to destroy.
 *
 * @par Notes
 * @parblock
 * The lock must have been dynamically initialized by
 * rtems_interrupt_lock_initialize(), statically defined by
 * RTEMS_INTERRUPT_LOCK_DEFINE(), or statically initialized by
 * RTEMS_INTERRUPT_LOCK_INITIALIZER().
 *
 * Concurrent lock use during the destruction or concurrent destruction leads
 * to unpredictable results.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define rtems_interrupt_lock_destroy( _lock ) _ISR_lock_Destroy( _lock )

/* Generated from spec:/rtems/intr/if/lock-acquire */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Acquires the ISR lock.
 *
 * @param[in,out] _lock is the ISR lock to acquire.
 *
 * @param[out] _lock_context is the ISR lock context.  This lock context shall
 *   be used to release the lock by calling rtems_interrupt_lock_release().
 *
 * This directive acquires the ISR lock specified by ``_lock`` using the lock
 * context provided by ``_lock_context``.  Maskable interrupts will be disabled
 * on the current processor.
 *
 * @par Notes
 * @parblock
 * A caller-specific lock context shall be provided for each acquire/release
 * pair, for example an automatic variable.
 *
 * Where the system was built with SMP support enabled, this directive acquires
 * an SMP lock.  An attempt to recursively acquire the lock may result in an
 * infinite loop with maskable interrupts disabled.
 *
 * This directive establishes a non-preemptive critical section with system
 * wide mutual exclusion on the local node in all RTEMS build configurations.
 *
 * @code
 * #include <rtems.h>
 *
 * void critical_section( rtems_interrupt_lock *lock )
 * {
 *   rtems_interrupt_lock_context lock_context;
 *
 *   rtems_interrupt_lock_acquire( lock, &lock_context );
 *
 *   // Here is the critical section.  Maskable interrupts are disabled.
 *   // Where the system was built with SMP support enabled, this section
 *   // is protected by an SMP lock.
 *
 *   rtems_interrupt_lock_release( lock, &lock_context );
 * }
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define rtems_interrupt_lock_acquire( _lock, _lock_context ) \
  _ISR_lock_ISR_disable_and_acquire( _lock, _lock_context )

/* Generated from spec:/rtems/intr/if/lock-release */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Releases the ISR lock.
 *
 * @param[in,out] _lock is the ISR lock to release.
 *
 * @param[in,out] _lock_context is the ISR lock context.  This lock context
 *   shall have been used to acquire the lock by calling
 *   rtems_interrupt_lock_acquire().
 *
 * This directive releases the ISR lock specified by ``_lock`` using the lock
 * context provided by ``_lock_context``.  The previous interrupt level will be
 * restored on the current processor.
 *
 * @par Notes
 * @parblock
 * The lock context shall be the one used to acquire the lock, otherwise the
 * result is unpredictable.
 *
 * Where the system was built with SMP support enabled, this directive releases
 * an SMP lock.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * While at least one maskable interrupt is pending, when the directive
 *   enables maskable interrupts, the pending interrupts are immediately
 *   serviced.  The interrupt service routines may unblock higher priority
 *   tasks which may preempt the calling task.
 * @endparblock
 */
#define rtems_interrupt_lock_release( _lock, _lock_context ) \
  _ISR_lock_Release_and_ISR_enable( _lock, _lock_context )

/* Generated from spec:/rtems/intr/if/lock-acquire-isr */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Acquires the ISR lock from within an ISR.
 *
 * @param[in,out] _lock is the ISR lock to acquire within an ISR.
 *
 * @param[out] _lock_context is the ISR lock context.  This lock context shall
 *   be used to release the lock by calling rtems_interrupt_lock_release_isr().
 *
 * This directive acquires the ISR lock specified by ``_lock`` using the lock
 * context provided by ``_lock_context``.  The interrupt level will remain
 * unchanged.
 *
 * @par Notes
 * @parblock
 * A caller-specific lock context shall be provided for each acquire/release
 * pair, for example an automatic variable.
 *
 * Where the system was built with SMP support enabled, this directive acquires
 * an SMP lock.  An attempt to recursively acquire the lock may result in an
 * infinite loop.
 *
 * This directive is intended for device drivers and should be called from the
 * corresponding interrupt service routine.
 *
 * In case the corresponding interrupt service routine can be interrupted by
 * higher priority interrupts and these interrupts enter the critical section
 * protected by this lock, then the result is unpredictable.  This directive
 * may be used under specific circumstances as an optimization.  In doubt, use
 * rtems_interrupt_lock_acquire() and rtems_interrupt_lock_release().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#if defined(RTEMS_SMP)
  #define rtems_interrupt_lock_acquire_isr( _lock, _lock_context ) \
    _SMP_lock_Acquire( \
      &( _lock )->Lock, \
      &( _lock_context )->Lock_context \
    )
#else
  #define rtems_interrupt_lock_acquire_isr( _lock, _lock_context ) \
    do { (void) _lock_context; } while ( 0 )
#endif

/* Generated from spec:/rtems/intr/if/lock-release-isr */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Releases the ISR lock from within an ISR.
 *
 * @param[in,out] _lock is the ISR lock to release within an ISR.
 *
 * @param[in,out] _lock_context is the ISR lock context.  This lock context
 *   shall have been used to acquire the lock by calling
 *   rtems_interrupt_lock_acquire_isr().
 *
 * This directive releases the ISR lock specified by ``_lock`` using the lock
 * context provided by ``_lock_context``.  The interrupt level will remain
 * unchanged.
 *
 * @par Notes
 * @parblock
 * The lock context shall be the one used to acquire the lock, otherwise the
 * result is unpredictable.
 *
 * Where the system was built with SMP support enabled, this directive releases
 * an SMP lock.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#if defined(RTEMS_SMP)
  #define rtems_interrupt_lock_release_isr( _lock, _lock_context ) \
    _SMP_lock_Release( \
      &( _lock )->Lock, \
      &( _lock_context )->Lock_context \
    )
#else
  #define rtems_interrupt_lock_release_isr( _lock, _lock_context ) \
    do { (void) _lock_context; } while ( 0 )
#endif

/* Generated from spec:/rtems/intr/if/lock-isr-disable */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Disables maskable interrupts on the current processor.
 *
 * @param[out] _lock_context is the ISR lock context for an acquire and release
 *   pair.
 *
 * This directive disables maskable interrupts on the current processor and
 * stores the previous interrupt level in ``_lock_context``.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define rtems_interrupt_lock_interrupt_disable( _lock_context ) \
  _ISR_lock_ISR_disable( _lock_context )

/* Generated from spec:/rtems/intr/if/lock-declare */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Declares an ISR lock object.
 *
 * @param _specifier is the storage-class specifier for the ISR lock to
 *   declare, for example ``extern`` or ``static``.
 *
 * @param _designator is the ISR lock object designator.
 *
 * @par Notes
 * Do not add a ";" after this macro.
 */
#define RTEMS_INTERRUPT_LOCK_DECLARE( _specifier, _designator ) \
  ISR_LOCK_DECLARE( _specifier, _designator )

/* Generated from spec:/rtems/intr/if/lock-define */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Defines an ISR lock object.
 *
 * @param _specifier is the storage-class specifier for the ISR lock to
 *   declare, for example ``extern`` or ``static``.
 *
 * @param _designator is the ISR lock object designator.
 *
 * @param _name is the ISR lock name.  It shall be a string.  The name is only
 *   used where the system was built with profiling support enabled.
 *
 * @par Notes
 * @parblock
 * Do not add a ";" after this macro.
 *
 * ISR locks may also be dynamically initialized by
 * rtems_interrupt_lock_initialize() or statically by
 * RTEMS_INTERRUPT_LOCK_INITIALIZER().
 * @endparblock
 */
#define RTEMS_INTERRUPT_LOCK_DEFINE( _specifier, _designator, _name ) \
  ISR_LOCK_DEFINE( _specifier, _designator, _name )

/* Generated from spec:/rtems/intr/if/lock-initializer */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Statically initializes an ISR lock object.
 *
 * @param _name is the ISR lock name.  It shall be a string.  The name is only
 *   used where the system was built with profiling support enabled.
 *
 * @par Notes
 * ISR locks may also be dynamically initialized by
 * rtems_interrupt_lock_initialize() or statically defined by
 * RTEMS_INTERRUPT_LOCK_DEFINE().
 */
#define RTEMS_INTERRUPT_LOCK_INITIALIZER( _name ) ISR_LOCK_INITIALIZER( _name )

/* Generated from spec:/rtems/intr/if/lock-member */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Defines an ISR lock member.
 *
 * @param _designator is the ISR lock member designator.
 *
 * @par Notes
 * Do not add a ";" after this macro.
 */
#define RTEMS_INTERRUPT_LOCK_MEMBER( _designator ) \
  ISR_LOCK_MEMBER( _designator )

/* Generated from spec:/rtems/intr/if/lock-reference */

/**
 * @ingroup RTEMSAPIClassicIntr
 *
 * @brief Defines an ISR lock object reference.
 *
 * @param _designator is the ISR lock reference designator.
 *
 * @param _target is the target object to reference.
 *
 * @par Notes
 * Do not add a ";" after this macro.
 */
#define RTEMS_INTERRUPT_LOCK_REFERENCE( _designator, _target ) \
  ISR_LOCK_REFERENCE( _designator, _target )

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_INTR_H */
