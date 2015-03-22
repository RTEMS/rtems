/**
 *  @file  rtems/score/interr.h
 *
 *  @brief Constants and Prototypes Related to the Internal Error Handler
 *
 *  This include file contains constants and prototypes related
 *  to the Internal Error Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_INTERR_H
#define _RTEMS_SCORE_INTERR_H

#include <stdbool.h>
#include <stdint.h>

#include <rtems/system.h>

/**
 *  @defgroup ScoreIntErr Internal Error Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Semaphore services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief This type lists the possible sources from which an error
 *  can be reported.
 */
typedef enum {
  /**
   * @brief Errors of the core system.
   *
   * @see Internal_errors_Core_list.
   */
  INTERNAL_ERROR_CORE,

  /**
   * @brief Errors of the RTEMS API.
   */
  INTERNAL_ERROR_RTEMS_API,

  /**
   * @brief Errors of the POSIX API.
   */
  INTERNAL_ERROR_POSIX_API,

  /**
   * @brief Fatal source for the block device cache.
   *
   * @see rtems_bdbuf_fatal_code.
   */
  RTEMS_FATAL_SOURCE_BDBUF,

  /**
   * @brief Fatal source for application specific errors.
   *
   * The fatal code is application specific.
   */
  RTEMS_FATAL_SOURCE_APPLICATION,

  /**
   * @brief Fatal source of exit().
   *
   * The fatal code is the exit() status code.
   */
  RTEMS_FATAL_SOURCE_EXIT,

  /**
   * @brief Fatal source for BSP errors.
   *
   * The fatal codes are defined in <bsp/fatal.h>.  Examples are interrupt and
   * exception initialization.
   *
   * @see bsp_fatal_code and bsp_fatal().
   */
  RTEMS_FATAL_SOURCE_BSP,

  /**
   * @brief Fatal source of assert().
   *
   * The fatal code is the pointer value of the assert context.
   *
   * @see rtems_assert_context.
   */
  RTEMS_FATAL_SOURCE_ASSERT,

  /**
   * @brief Fatal source of the stack checker.
   *
   * The fatal code is the object name of the executing task.
   */
  RTEMS_FATAL_SOURCE_STACK_CHECKER,

  /**
   * @brief Fatal source of the exceptions.
   *
   * The fatal code is the pointer value of the exception frame pointer.
   *
   * @see rtems_exception_frame and rtems_exception_frame_print().
   */
  RTEMS_FATAL_SOURCE_EXCEPTION,

  /**
   * @brief Fatal source of SMP domain.
   *
   * @see SMP_Fatal_code.
   */
  RTEMS_FATAL_SOURCE_SMP,

  /**
   * @brief The last available fatal source.
   *
   * This enum value ensures that the enum type needs at least 32-bits for
   * architectures with short enums.
   */
  RTEMS_FATAL_SOURCE_LAST = 0xffffffff
} Internal_errors_Source;

/**
 *  A list of errors which are generated internally by the executive core.
 */
typedef enum {
  INTERNAL_ERROR_NO_CONFIGURATION_TABLE,
  INTERNAL_ERROR_NO_CPU_TABLE,
  INTERNAL_ERROR_TOO_LITTLE_WORKSPACE,
  INTERNAL_ERROR_WORKSPACE_ALLOCATION,
  INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL,
  INTERNAL_ERROR_THREAD_EXITTED,
  INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION,
  INTERNAL_ERROR_INVALID_NODE,
  INTERNAL_ERROR_NO_MPCI,
  INTERNAL_ERROR_BAD_PACKET,
  INTERNAL_ERROR_OUT_OF_PACKETS,
  INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS,
  INTERNAL_ERROR_OUT_OF_PROXIES,
  INTERNAL_ERROR_INVALID_GLOBAL_ID,
  INTERNAL_ERROR_BAD_STACK_HOOK,
  INTERNAL_ERROR_BAD_ATTRIBUTES,
  INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY,
  OBSOLETE_INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL,
  INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE,
  INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0,
  OBSOLETE_INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP,
  INTERNAL_ERROR_GXX_KEY_ADD_FAILED,
  INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED,
  INTERNAL_ERROR_NO_MEMORY_FOR_HEAP,
  INTERNAL_ERROR_CPU_ISR_INSTALL_VECTOR,
  INTERNAL_ERROR_RESOURCE_IN_USE
} Internal_errors_Core_list;

typedef CPU_Uint32ptr Internal_errors_t;

/**
 *  This type holds the fatal error information.
 */
typedef struct {
  /** This is the source of the error. */
  Internal_errors_Source  the_source;
  /** This indicates if the error is internal of external. */
  bool                    is_internal;
  /** This is the error code. */
  Internal_errors_t       the_error;
} Internal_errors_Information;

/**
 *  When a fatal error occurs, the error information is stored here.
 */
extern Internal_errors_Information _Internal_errors_What_happened;

/**
 * @brief Initiates system termination.
 *
 * This routine is invoked when the application or the executive itself
 * determines that a fatal error has occurred or a final system state is
 * reached (for example after exit()).
 *
 * The first action is to disable interrupts.
 *
 * The second action of this function is to call the fatal handler of the user
 * extensions.  For the initial extensions the following conditions are
 * required
 * - a valid stack pointer and enough stack space,
 * - a valid code memory, and
 * - valid read-only data.
 *
 * For the initial extensions the read-write data (including BSS segment) is
 * not required on single processor configurations.  On SMP configurations
 * however the read-write data must be initialized since this function must
 * determine the state of the other processors and request them to shut-down if
 * necessary.
 *
 * Non-initial extensions require in addition valid read-write data.  The BSP
 * may install an initial extension that performs a system reset.  In this case
 * the non-initial extensions will be not called.
 *
 * Once all fatal handler executed the error information will be stored to
 * _Internal_errors_What_happened and the system state is set to
 * SYSTEM_STATE_TERMINATED.
 *
 * The final step is to call the CPU specific _CPU_Fatal_halt().
 *
 * @param[in] the_source The fatal source indicating the subsystem the fatal
 * condition originated in.
 * @param[in] is_internal Indicates if the fatal condition was generated
 * internally to the executive.
 * @param[in] the_error The fatal error code.  This value must be interpreted
 * with respect to the source.
 *
 * @see rtems_fatal_error_occurred() and rtems_fatal().
 */
void _Terminate(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  Internal_errors_t       the_error
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
