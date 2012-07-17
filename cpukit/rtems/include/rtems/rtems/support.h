/**
 * @file
 *
 * @ingroup ClassicRTEMS
 *
 * @ingroup ClassicRTEMSWorkspace
 *
 * @brief Classic API support.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SUPPORT_H
#define _RTEMS_RTEMS_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>

/**
 * @addtogroup ClassicRTEMS
 *
 * @{
 */

/**
 * @brief Returns the number of micro seconds for the milli seconds value @a _ms.
 */
#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) \
        TOD_MILLISECONDS_TO_MICROSECONDS(_ms)

/**
 * @brief Returns the number of ticks for the milli seconds value @a _ms.
 */
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
       (TOD_MILLISECONDS_TO_MICROSECONDS(_ms) / \
          rtems_configuration_get_microseconds_per_tick())

/**
 * @brief Returns the number of ticks for the micro seconds value @a _us.
 */
#define RTEMS_MICROSECONDS_TO_TICKS(_us) \
       ((_us) / rtems_configuration_get_microseconds_per_tick())

/** @} */

/**
 * @defgroup ClassicRTEMSWorkspace Workspace
 *
 * @ingroup ClassicRTEMS
 *
 * Workspace definitions.
 *
 * @{
 */

/**
 * @brief Gets workspace information.
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
 * @brief Allocates memory from the workspace.
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
 * @brief Frees memory allocated from the workspace.
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
 * Afterward the heap has at most @a block_count allocateable blocks of sizes
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
 * @brief Frees space of a greedy allocation.
 *
 * The @a opaque argument must be the return value of
 * rtems_workspace_greedy_allocate().
 */
void rtems_workspace_greedy_free( void *opaque );

/** @} */

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/support.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
