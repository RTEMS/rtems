/**
 * @file
 *
 * @defgroup ClassicRTEMSWorkspace Workspace
 *
 * @ingroup ClassicRTEMS
 * @brief Classic API support.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SUPPORT_H
#define _RTEMS_RTEMS_SUPPORT_H

#include <rtems/rtems/types.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicRTEMS
 */
/**@{**/

/**
 * @brief Returns the number of micro seconds for the milli seconds value @a _ms.
 */
#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) ((uint32_t)(_ms) * 1000UL)

/**
 * @brief Returns the number of ticks for the milli seconds value @a _ms.
 */
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
       (RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) / \
          rtems_configuration_get_microseconds_per_tick())

/**
 * @brief Returns the number of ticks for the micro seconds value @a _us.
 */
#define RTEMS_MICROSECONDS_TO_TICKS(_us) \
       ((_us) / rtems_configuration_get_microseconds_per_tick())

/**
 * @brief Returns @c true if the name is valid, and @c false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_is_name_valid (
  rtems_name name
)
{
  return ( name != 0 );
}

/**
 * @brief Breaks the object name into the four component characters @a c1,
 * @a c2, @a c3, and @a c4.
 */
RTEMS_INLINE_ROUTINE void rtems_name_to_characters(
  rtems_name    name,
  char         *c1,
  char         *c2,
  char         *c3,
  char         *c4
)
{
  *c1 = (char) ((name >> 24) & 0xff);
  *c2 = (char) ((name >> 16) & 0xff);
  *c3 = (char) ((name >> 8) & 0xff);
  *c4 = (char) ( name & 0xff);
}

/** @} */

/**
 * @defgroup ClassicRTEMSWorkspace Workspace
 *
 * @ingroup ClassicRTEMS
 *
 * Workspace definitions.
 */
/**@{**/

/**
 * @brief Gets Workspace Information
 *
 * Returns information about the heap that is used as the RTEMS Executive
 * Workspace in @a the_info.
 *
 * Returns @c true if successful, and @a false otherwise.
 */
bool rtems_workspace_get_information(
  Heap_Information_block  *the_info
);

/**
 * @brief Allocates Memory from the Workspace
 *
 * A number of @a bytes bytes will be allocated from the RTEMS Executive
 * Workspace and returned in @a pointer.
 *
 * Returns @c true if successful, and @a false otherwise.
 */
bool rtems_workspace_allocate(
  size_t   bytes,
  void   **pointer
);

/**
 * @brief Frees Memory Allocated from the Workspace
 *
 * This frees the memory indicated by @a pointer that was allocated from the
 * RTEMS Executive Workspace.
 *
 * Returns @c true if successful, and @a false otherwise.
 */
bool rtems_workspace_free(
  void *pointer
);

/**
 * @brief Greedy allocate that empties the workspace.
 *
 * Afterwards the heap has at most @a block_count allocatable blocks of sizes
 * specified by @a block_sizes.  The @a block_sizes must point to an array with
 * @a block_count members.  All other blocks are used.
 *
 * @see rtems_workspace_greedy_free().
 */
void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
);

/**
 * @brief Greedy allocate all blocks except the largest free block.
 *
 * Afterwards the heap has at most one allocatable block.  This block is the
 * largest free block if it exists.  The allocatable size of this block is
 * stored in @a allocatable_size.  All other blocks are used.
 *
 * @see rtems_workspace_greedy_free().
 */
void *rtems_workspace_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
);

/**
 * @brief Frees space of a greedy allocation.
 *
 * The @a opaque argument must be the return value of
 * rtems_workspace_greedy_allocate() or
 * rtems_workspace_greedy_allocate_all_except_largest().
 */
void rtems_workspace_greedy_free( void *opaque );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
