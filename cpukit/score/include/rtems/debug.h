/**
 *  @file  rtems/debug.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_DEBUG_H
#define _RTEMS_DEBUG_H

#include <rtems/score/basedefs.h>

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
 *  This routine performs the initialization necessary for this manager.
 */
void _Debug_Manager_initialization( void );

/**
 *  This routine enables the specified types of debug checks.
 */
void rtems_debug_enable(
  rtems_debug_control  to_be_enabled
);

/**
 *  This routine disables the specified types of debug checks.
 */
void rtems_debug_disable(
  rtems_debug_control  to_be_disabled
);

/**
 *  This routine returns TRUE if the requested debug level is
 *  enabled, and FALSE otherwise.
 */
bool rtems_debug_is_enabled(
  rtems_debug_control  level
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
