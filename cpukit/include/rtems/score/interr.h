/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreIntErr
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreIntErr.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_INTERR_H
#define _RTEMS_SCORE_INTERR_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreIntErr Internal Error Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Internal Error Handler implementation.
 *
 * This handler encapsulates functionality to terminate the system.
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
   * @brief Fatal source for spurious interrupts.
   *
   * The fatal code is the interrupt vector number of the spurious interrupt.
   */
  RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT = 14,

  /**
   * @brief The last available fatal source.
   *
   * This enum value ensures that the enum type needs at least 32-bits for
   * architectures with short enums.
   */
  RTEMS_FATAL_SOURCE_LAST = 0x7fffffff
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
  /* INTERNAL_ERROR_BAD_STACK_HOOK = 14, */
  /* INTERNAL_ERROR_BAD_ATTRIBUTES = 15, */
  /* INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY = 16, */
  /* INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL = 17, */
  /* INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_FROM_BAD_STATE = 18, */
  /* INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0 = 19, */
  /* INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP = 20, */
  /* INTERNAL_ERROR_GXX_KEY_ADD_FAILED = 21, */
  /* INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED = 22, */
  INTERNAL_ERROR_NO_MEMORY_FOR_HEAP = 23,
  INTERNAL_ERROR_CPU_ISR_INSTALL_VECTOR = 24,
  INTERNAL_ERROR_RESOURCE_IN_USE = 25,
  /* INTERNAL_ERROR_RTEMS_INIT_TASK_ENTRY_IS_NULL = 26, */
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
  INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA = 40,
  INTERNAL_ERROR_TOO_LARGE_TLS_SIZE = 41,
  INTERNAL_ERROR_RTEMS_INIT_TASK_CONSTRUCT_FAILED = 42,
  INTERNAL_ERROR_IDLE_THREAD_CREATE_FAILED = 43,
  INTERNAL_ERROR_NO_MEMORY_FOR_IDLE_TASK_STORAGE = 44,
  INTERNAL_ERROR_IDLE_THREAD_STACK_TOO_SMALL = 45,
  INTERNAL_ERROR_CANNOT_DISABLE_DATA_CACHE = 46,
  INTERNAL_ERROR_LIBIO_STDIN_FD_OPEN_FAILED = 47,
} Internal_errors_Core_list;

typedef CPU_Uint32ptr Internal_errors_t;

/**
 * @brief Initiates the system termination.
 *
 * This handler is invoked to terminate the system.  It is called by all
 * services which determine that a system termination is required.  For
 * example, it is called by all higher level directives which announce a fatal
 * error like rtems_fatal() and exit().
 *
 * The first action of the system termination handler is to disable maskable
 * interrupts.  This ensures that interrupts on this processor do not interfere
 * with the system termination procedure.  This reduces the likelihood to end
 * up in a recursive system termination procedure.
 *
 * The second action of the system termination handler is to call the fatal
 * extensions of the user extensions.
 *
 * The fatal extensions are called with three parameters:
 *
 * - the fatal source,
 *
 * - a legacy parameter which is always set to false, and
 *
 * - an error code with a fatal source dependent content.
 *
 * The fatal extensions of the initial extension sets are invoked first.  For
 * them, the following execution environment is required
 *
 * - a valid stack pointer and enough stack space,
 *
 * - a valid code memory, and
 *
 * - valid read-only data.
 *
 * In uniprocessor configurations, the read-write data (including ``.bss``
 * segment) is not required.  In SMP configurations, however, the read-write
 * data must have been initialized to determine the state of the other
 * processors and request them to shut-down if necessary.  The board support
 * package (BSP) may install an initial extension that performs a system reset.
 * See the BSP documentation in the *RTEMS User Manual* for more information
 * how the system reset is done.  The BSP provided fatal extension can be
 * disabled by the #CONFIGURE_DISABLE_BSP_SETTINGS application configuration
 * option.  It is recommended to provide an application-specific fatal
 * extension using the #CONFIGURE_INITIAL_EXTENSIONS application configuration
 * option.
 *
 * In certain error conditions, it may be unreliable to carry out the
 * following steps of the termination procedure since the read-write data may
 * be corrupt.  One of the fatal extensions of the initial extension set should
 * reset the system to stop the system termination procedure.
 *
 * After invoking the fatal extensions of the initial extension sets, the fatal
 * extensions of the dynamic extension sets are invoked.  For this procedure
 * valid read-write data is required.
 *
 * The last action of the system termination handler is to execute the CPU port
 * provided idle loop _CPU_Thread_Idle_body() with maskable interrupts
 * disabled.  Please note, that properly configured applications should not
 * reach this point.
 *
 * @param the_source is the fatal source indicating the subsystem the fatal
 *   condition originated in.
 *
 * @param the_error is the fatal error code.  This value must be interpreted
 * with respect to the source.
 *
 * @see rtems_fatal() and _Internal_error().
 */
RTEMS_NO_RETURN void _Terminate(
  Internal_errors_Source  the_source,
  Internal_errors_t       the_error
);

/**
 * @brief Terminates the system with an INTERNAL_ERROR_CORE fatal source and
 * the specified core error code.
 *
 * @param core_error The core error code.
 *
 * @see _Terminate().
 */
RTEMS_NO_RETURN void _Internal_error( Internal_errors_Core_list core_error );

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
