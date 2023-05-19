/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreStack
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreStack which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
 * Copyright (C) 1989, 2021 On-Line Applications Research Corporation (OAR)
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

#ifndef _RTEMS_SCORE_STACK_H
#define _RTEMS_SCORE_STACK_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreStack Stack Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Stack Handler implementation.
 *
 * This handler encapsulates functionality which is used in the management
 * of thread stacks.  It provides mechanisms which can be used to initialize
 * and utilize stacks.
 *
 * @{
 */

/**
 *  The following constant defines the minimum stack size which every
 *  thread must exceed.
 */
#define STACK_MINIMUM_SIZE  CPU_STACK_MINIMUM_SIZE

/**
 *  The following defines the control block used to manage each stack.
 */
typedef struct {
  /** This is the stack size. */
  size_t      size;
  /** This is the low memory address of stack. */
  void       *area;
}   Stack_Control;

/**
 * @brief The stack allocator initialization handler.
 *
 * @param stack_space_size The size of the stack space in bytes.
 */
typedef void ( *Stack_Allocator_initialize )( size_t stack_space_size );

/**
 * @brief Stack allocator allocate handler.
 *
 * @param stack_size The size of the stack area to allocate in bytes.
 *
 * @retval NULL Not enough memory.
 * @retval other Pointer to begin of stack area.
 */
typedef void *( *Stack_Allocator_allocate )( size_t stack_size );

/**
 * @brief Stack allocator free handler.
 *
 * @param] addr A pointer to previously allocated stack area or NULL.
 */
typedef void ( *Stack_Allocator_free )( void *addr );

/**
 * @brief Stack allocator allocate for idle handler.
 *
 * The allocate for idle handler is optional even when the user thread stack
 * allocator and deallocator are configured.
 *
 * @param cpu is the index of the CPU for the IDLE thread using this stack.
 *
 * @param stack_size[in, out] is pointer to a size_t object.  On function
 *   entry, the object contains the proposed size of the stack area to allocate
 *   in bytes.  The proposed size does not take the actual thread-local storage
 *   size of the application into account.  The stack allocator can modify the
 *   size to ensure that there is enough space available in the stack area for
 *   the thread-local storage.
 *
 * @retval NULL There was not enough memory available to allocate a stack area.
 *
 * @return Returns the pointer to begin of the allocated stack area.
 */
typedef void *( *Stack_Allocator_allocate_for_idle )(
  uint32_t  cpu,
  size_t   *stack_size
);

/**
 * @brief The minimum stack size.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern uint32_t rtems_minimum_stack_size;

/**
 * @brief The configured stack space size.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const uintptr_t _Stack_Space_size;

/**
 * @brief Indicates if the stack allocator avoids the workspace.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const bool _Stack_Allocator_avoids_workspace;

/**
 * @brief The stack allocator initialization handler.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const Stack_Allocator_initialize _Stack_Allocator_initialize;

/**
 * @brief The stack allocator allocate handler.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const Stack_Allocator_allocate _Stack_Allocator_allocate;

/**
 * @brief The stack allocator free handler.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const Stack_Allocator_free _Stack_Allocator_free;

/**
 * @brief Do the stack allocator initialization during system initialize.
 *
 * This function is used to initialize application provided stack allocators.
 */
void _Stack_Allocator_do_initialize( void );

/**
 * @brief Allocates the IDLE thread storage area from the workspace.
 *
 * If the thread storage area cannot be allocated, then the
 * ::INTERNAL_ERROR_NO_MEMORY_FOR_IDLE_TASK_STACK fatal error will occur.
 *
 * @param unused is an unused parameter.
 *
 * @param stack_size[in] is pointer to a size_t object.  On function entry, the
 *   object contains the size of the task storage area to allocate in bytes.
 *
 * @return Returns a pointer to the begin of the allocated task storage area.
 */
void *_Stack_Allocator_allocate_for_idle_workspace(
  uint32_t  unused,
  size_t   *storage_size
);

/**
 * @brief The size in bytes of the idle thread storage area used by
 *   _Stack_Allocator_allocate_for_idle_static().
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const size_t _Stack_Allocator_allocate_for_idle_storage_size;

/**
 * @brief The thread storage areas used by
 *   _Stack_Allocator_allocate_for_idle_static().
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern char _Stack_Allocator_allocate_for_idle_storage_areas[];

/**
 * @brief Allocates the IDLE thread storage from the memory statically
 *   allocated by <rtems/confdefs.h>.
 *
 * @param cpu_index is the index of the CPU for the IDLE thread using this stack.
 *
 * @param stack_size[out] is pointer to a size_t object.  On function return, the
 *   object value is set to the value of
 *   ::_Stack_Allocator_allocate_for_idle_storage_size.
 *
 * @return Returns a pointer to the begin of the allocated task storage area.
 */
void *_Stack_Allocator_allocate_for_idle_static(
  uint32_t  cpu_index,
  size_t   *storage_size
);

/**
 * @brief The stack allocator allocate stack for idle thread handler.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const Stack_Allocator_allocate_for_idle
  _Stack_Allocator_allocate_for_idle;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
