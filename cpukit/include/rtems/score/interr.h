/**
 * @file
 *
 * @ingroup RTEMSScoreIntErr
 *
 * @brief Constants and Prototypes Related to the Internal Error Handler
 *
 * This include file contains constants and prototypes related
 * to the Internal Error Handler.
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

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreIntErr Internal Error Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief Internal Error Handler
 *
 * This handler encapsulates functionality which provides the foundation
 * Semaphore services used in all of the APIs supported by RTEMS.
 *
 * @{
 */

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
  INTERNAL_ERROR_CORE = 0,

  /**
   * @brief Errors of the RTEMS API.
   */
  INTERNAL_ERROR_RTEMS_API = 1,

  /**
   * @brief Errors of the POSIX API.
   */
  INTERNAL_ERROR_POSIX_API = 2,

  /**
   * @brief Fatal source for the block device cache.
   *
   * @see rtems_bdbuf_fatal_code.
   */
  RTEMS_FATAL_SOURCE_BDBUF = 3,

  /**
   * @brief Fatal source for application specific errors.
   *
   * The fatal code is application specific.
   */
  RTEMS_FATAL_SOURCE_APPLICATION = 4,

  /**
   * @brief Fatal source of exit().
   *
   * The fatal code is the exit() status code.
   */
  RTEMS_FATAL_SOURCE_EXIT = 5,

  /**
   * @brief Fatal source for BSP errors.
   *
   * The fatal codes are defined in <bsp/fatal.h>.  Examples are interrupt and
   * exception initialization.
   *
   * @see bsp_fatal_code and bsp_fatal().
   */
  RTEMS_FATAL_SOURCE_BSP = 6,

  /**
   * @brief Fatal source of assert().
   *
   * The fatal code is the pointer value of the assert context.
   *
   * @see rtems_assert_context.
   */
  RTEMS_FATAL_SOURCE_ASSERT = 7,

  /**
   * @brief Fatal source of the stack checker.
   *
   * The fatal code is the object name of the executing task.
   */
  RTEMS_FATAL_SOURCE_STACK_CHECKER = 8,

  /**
   * @brief Fatal source of the exceptions.
   *
   * The fatal code is the pointer value of the exception frame pointer.
   *
   * @see rtems_exception_frame and rtems_exception_frame_print().
   */
  RTEMS_FATAL_SOURCE_EXCEPTION = 9,

  /**
   * @brief Fatal source of SMP domain.
   *
   * @see SMP_Fatal_code.
   */
  RTEMS_FATAL_SOURCE_SMP = 10,

  /**
   * @brief Fatal source of rtems_panic().
   *
   * @see rtem
   */
  RTEMS_FATAL_SOURCE_PANIC = 11,

  /**
   * @brief Fatal source for invalid C program heap frees via free().
   *
   * The fatal code is the bad pointer.
   */
  RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE = 12,

  /**
   * @brief Fatal source for heap errors.
   *
   * The fatal code is the address to a heap error context (Heap_Error_context).
   */
  RTEMS_FATAL_SOURCE_HEAP = 13,

  /**
   * @brief The last available fatal source.
   *
   * This enum value ensures that the enum type needs at least 32-bits for
   * architectures with short enums.
   */
  RTEMS_FATAL_SOURCE_LAST = 0xffffffff
} Internal_errors_Source;

/**
 * @brief A list of errors which are generated internally by the executive
 * core.
 *
 * Do not re-use numbers of obsolete error codes.  Comment no longer used
 * error codes and do not uncomment commented or obsolete error codes.
 */
typedef enum {
  /* INTERNAL_ERROR_NO_CONFIGURATION_TABLE = 0, */
  /* INTERNAL_ERROR_NO_CPU_TABLE = 1, */
  INTERNAL_ERROR_TOO_LITTLE_WORKSPACE = 2,
  /* INTERNAL_ERROR_WORKSPACE_ALLOCATION = 3, */
  /* INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL = 4, */
  INTERNAL_ERROR_THREAD_EXITTED = 5,
  INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION = 6,
  INTERNAL_ERROR_INVALID_NODE = 7,
  INTERNAL_ERROR_NO_MPCI = 8,
  INTERNAL_ERROR_BAD_PACKET = 9,
  INTERNAL_ERROR_OUT_OF_PACKETS = 10,
  INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS = 11,
  INTERNAL_ERROR_OUT_OF_PROXIES = 12,
  INTERNAL_ERROR_INVALID_GLOBAL_ID = 13,
  INTERNAL_ERROR_BAD_STACK_HOOK = 14,
  /* INTERNAL_ERROR_BAD_ATTRIBUTES = 15, */
  /* INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY = 16, */
  /* INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL = 17, */
  /* INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_FROM_BAD_STATE = 18, */
  /* INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0 = 19, */
  /* INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP = 20, */
  INTERNAL_ERROR_GXX_KEY_ADD_FAILED = 21,
  INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED = 22,
  INTERNAL_ERROR_NO_MEMORY_FOR_HEAP = 23,
  INTERNAL_ERROR_CPU_ISR_INSTALL_VECTOR = 24,
  INTERNAL_ERROR_RESOURCE_IN_USE = 25,
  INTERNAL_ERROR_RTEMS_INIT_TASK_ENTRY_IS_NULL = 26,
  /* INTERNAL_ERROR_POSIX_INIT_THREAD_ENTRY_IS_NULL = 27, */
  INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK = 28,
  INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_STICKY_FROM_BAD_STATE = 29,
  INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL = 30,
  INTERNAL_ERROR_BAD_THREAD_DISPATCH_ENVIRONMENT = 31,
  INTERNAL_ERROR_RTEMS_INIT_TASK_CREATE_FAILED = 32,
  INTERNAL_ERROR_POSIX_INIT_THREAD_CREATE_FAILED = 33,
  /* INTERNAL_ERROR_LIBIO_USER_ENV_KEY_CREATE_FAILED = 34, */
  /* INTERNAL_ERROR_LIBIO_SEM_CREATE_FAILED = 35, */
  INTERNAL_ERROR_LIBIO_STDOUT_FD_OPEN_FAILED = 36,
  INTERNAL_ERROR_LIBIO_STDERR_FD_OPEN_FAILED = 37,
  INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT = 38,
  INTERNAL_ERROR_ARC4RANDOM_GETENTROPY_FAIL = 39,
  INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA = 40
} Internal_errors_Core_list;

typedef CPU_Uint32ptr Internal_errors_t;

/**
 *  This type holds the fatal error information.
 */
typedef struct {
  /** This is the source of the error. */
  Internal_errors_Source  the_source;
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
 * The first action of this function is to call the fatal handler of the user
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
 * @param the_source The fatal source indicating the subsystem the fatal
 * condition originated in.
 * @param the_error The fatal error code.  This value must be interpreted
 * with respect to the source.
 *
 * @see rtems_fatal() and _Internal_error().
 */
void _Terminate(
  Internal_errors_Source  the_source,
  Internal_errors_t       the_error
) RTEMS_NO_RETURN;

/**
 * @brief Terminates the system with an INTERNAL_ERROR_CORE fatal source and
 * the specified core error code.
 *
 * @param core_error The core error code.
 *
 * @see _Terminate().
 */
void _Internal_error( Internal_errors_Core_list core_error ) RTEMS_NO_RETURN;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
