/**
 *  @file  rtems/debug.h
 *
 *  @brief Information Related to the Debug Support Within RTEMS
 *
 *  This include file contains the information pertaining to the debug
 *  support within RTEMS.  It is currently cast in the form of a
 *  Manager since it is externally accessible.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_DEBUG_H
#define _RTEMS_DEBUG_H

#include <rtems/score/basedefs.h>

/**
 *  @defgroup ScoreDebug Debug Information
 *
 *  @ingroup Score
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following type is used to manage the debug mask.
 */
typedef uint32_t   rtems_debug_control;

/*
 *  These constants represent various classes of debugging.
 */

/** Macro which indicates that all debugging modes are enabled */
#define RTEMS_DEBUG_ALL_MASK 0xffffffff

/** Macro which indicates that debugging for heaps/regions is enabled */
#define RTEMS_DEBUG_REGION   0x00000001

/**
 *  This variable contains the current debug level.
 */
SCORE_EXTERN rtems_debug_control _Debug_Level;

/**
 *  @brief Initialize debug manager.
 */
void _Debug_Manager_initialization( void );

/**
 *  @brief Enable debugging.
 */
void rtems_debug_enable(
  rtems_debug_control  to_be_enabled
);

/**
 *  @brief Disable debugging.
 */
void rtems_debug_disable(
  rtems_debug_control  to_be_disabled
);

/**
 *  @brief Check if debug is enabled.
 *
 *  This routine returns TRUE if the requested debug level is
 *  enabled, and FALSE otherwise.
 */
bool rtems_debug_is_enabled(
  rtems_debug_control  level
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/* end of include file */
