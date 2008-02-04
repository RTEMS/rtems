/**
 * @file rtems/rtems/support.h
 */

/*
 *  This include file contains information about support functions for
 *  the RTEMS API.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_SUPPORT_H
#define _RTEMS_RTEMS_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>

/**  @brief milliseconds to microseconds
 *
 *  This is the public milliseconds to microseconds conversion.
 */
#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) \
        TOD_MILLISECONDS_TO_MICROSECONDS(_ms)

/** @brief milliseconds to ticks
 *
 *  This is the public milliseconds to ticks conversion.
 */
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
        TOD_MILLISECONDS_TO_TICKS(_ms)

/** @brief microseconds to ticks
 *  This is the public microseconds to tick conversion.
 */
#define RTEMS_MICROSECONDS_TO_TICKS(_ms) \
        TOD_MICROSECONDS_TO_TICKS(_ms)

/** @brief get workspace information
 *
 *  This returns information about the heap that is used as
 *  the RTEMS Executive Workspace.
 *
 *  @param[in] the_info
 *
 *  @return TRUE if successful
 */
boolean rtems_workspace_get_information(
  Heap_Information_block  *the_info
);

/** @brief get workspace information
 *
 *  This returns information about the heap that is used as
 *  the RTEMS Executive Workspace.
 *
 *  @param[in] bytes is the number of bytes to allocate 
 *  @param[in] pointer is the returned pointer to allocated memory
 *
 *  @return TRUE if successful
 */
boolean rtems_workspace_allocate(
  size_t   bytes,
  void   **pointer
);

/** @brief free workspace
 *
 *  This frees the workspace that was allocated from   
 *  the RTEMS Executive Workspace.
 *
 *  @param[in] pointer is the allocated workspace
 *
 *  @return TRUE if successful
 */
boolean rtems_workspace_free(
  void *pointer
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/support.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
