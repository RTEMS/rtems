/**
 * @file
 *
 * @ingroup RTEMSScoreStack
 *
 * @brief Inlined Routines from the Stack Handler
 *
 * This file contains the static inline implementation of the inlined
 * routines from the Stack Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_STACKIMPL_H
#define _RTEMS_SCORE_STACKIMPL_H

#include <rtems/score/stack.h>

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
RTEMS_INLINE_ROUTINE void _Stack_Initialize (
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
RTEMS_INLINE_ROUTINE uint32_t _Stack_Minimum (void)
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
 *
 * @retval true @a size is large enough.
 * @retval false @a size is not large enough.
 */
RTEMS_INLINE_ROUTINE bool _Stack_Is_enough (
  size_t size
)
{
  return ( size >= _Stack_Minimum() );
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
RTEMS_INLINE_ROUTINE size_t _Stack_Ensure_minimum (
  size_t size
)
{
  if ( size >= _Stack_Minimum() )
    return size;
  return _Stack_Minimum();
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
