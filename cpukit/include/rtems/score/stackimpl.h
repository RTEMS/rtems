/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreStack
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreStack which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#ifndef _RTEMS_SCORE_STACKIMPL_H
#define _RTEMS_SCORE_STACKIMPL_H

#include <rtems/score/stack.h>
#include <rtems/score/context.h>
#include <rtems/score/tls.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreStack
 *
 * @{
 */

/**
 * @brief Initializes stack with the given starting address and size.
 *
 * This routine initializes the_stack record to indicate that
 * size bytes of memory starting at starting_address have been
 * reserved for a stack.
 *
 * @param[out] the_stack The stack to initialize.
 * @param starting_address The starting_address for the new stack.
 * @param size The size of the stack in bytes.
 */
static inline void _Stack_Initialize (
  Stack_Control *the_stack,
  void          *starting_address,
  size_t         size
)
{
  the_stack->area = starting_address;
  the_stack->size = size;
}

/**
 * @brief Returns the minimum stack size.
 *
 * This function returns the minimum stack size configured
 * for this application.
 *
 * @return The minimum stack size.
 */
static inline uint32_t _Stack_Minimum (void)
{
  return rtems_minimum_stack_size;
}

/**
 * @brief Checks if the size is enough for a valid stack area on this processor.
 *
 * This function returns true if size bytes is enough memory for
 * a valid stack area on this processor, and false otherwise.
 *
 * @param size The stack size to check.
 * @param is_fp Indicates if the stack is for a floating-point thread.
 *
 * @retval true @a size is large enough.
 * @retval false @a size is not large enough.
 */
static inline bool _Stack_Is_enough(
  size_t size,
  bool   is_fp
)
{
  size_t minimum;

  minimum = _TLS_Get_allocation_size();
  minimum += _Stack_Minimum();

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( is_fp ) {
    minimum += CONTEXT_FP_SIZE;
  }
#endif

  return ( size >= minimum );
}

/**
 * @brief Returns the appropriate stack size for the requested size.
 *
 * This function returns the appropriate stack size given the requested
 * size.  If the requested size is below the minimum, then the minimum
 * configured stack size is returned.
 *
 * @param size The stack size to check.
 *
 * @return The appropriate stack size.
 */
static inline size_t _Stack_Ensure_minimum (
  size_t size
)
{
  if ( size >= _Stack_Minimum() )
    return size;
  return _Stack_Minimum();
}

/**
 * @brief Extends the stack size to account for additional data structures
 *   allocated in the thread storage area.
 *
 * @param stack_size is the stack size.
 *
 * @param is_fp shall be true, if the stack is for a floating-point thread,
 *   otherwise it shall be false.
 *
 * @return Returns the extended stack size.
 */
static inline size_t _Stack_Extend_size(
  size_t stack_size,
  bool   is_fp
)
{
  size_t extra_size;
  size_t allocator_overhead;

  extra_size = _TLS_Get_allocation_size();

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( is_fp ) {
    /* This addition cannot overflow since the TLS size cannot be that large */
    extra_size += CONTEXT_FP_SIZE;
  }
#else
  (void) is_fp;
#endif

  /*
   * In order to make sure that a user-provided stack size is the minimum which
   * can be allocated for the stack, we have to align it up to the next stack
   * boundary.
   */
  extra_size += CPU_STACK_ALIGNMENT - 1;

  /*
   * If the heap allocator does not meet the stack alignment requirement, then
   * we have to do the stack alignment manually in _Thread_Initialize() and
   * need to allocate extra space for this.
   */
  allocator_overhead = CPU_STACK_ALIGNMENT - CPU_HEAP_ALIGNMENT;

  if ( stack_size > SIZE_MAX - extra_size - allocator_overhead ) {
    /*
     * In case of an unsigned integer overflow, saturate at the maximum value.
     */
    return SIZE_MAX;
  }

  stack_size += extra_size;
  stack_size = RTEMS_ALIGN_DOWN( stack_size, CPU_STACK_ALIGNMENT );

  return stack_size + allocator_overhead;
}

/**
 * @brief Allocate the requested stack space.
 *
 * @param stack_size The stack space that is requested.
 *
 * @retval stack_area The allocated stack area.
 * @retval NULL The allocation failed.
 */
void *_Stack_Allocate( size_t stack_size );

/**
 * @brief Free the stack area allocated by _Stack_Allocate().
 *
 * Do nothing if the stack area is NULL.
 *
 * @param stack_area The stack area to free, or NULL.
 */
void _Stack_Free( void *stack_area );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
