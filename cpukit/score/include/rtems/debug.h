/*  debug.h
 *
 *  This include file contains the information pertaining to the debug
 *  support within RTEMS.  It is currently cast in the form of a 
 *  Manager since it is externally accessible.
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_DEBUG_h
#define __RTEMS_DEBUG_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type is used to manage the debug mask.
 */

typedef unsigned32 rtems_debug_control;

/*
 *  These constants represent various classes of debugging.
 */

#define RTEMS_DEBUG_ALL_MASK 0xffffffff
#define RTEMS_DEBUG_REGION   0x00000001

/*
 *  This variable contains the current debug level.
 */

SCORE_EXTERN rtems_debug_control _Debug_Level;

/*
 *  _Debug_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Debug_Manager_initialization( void );

/*
 *  rtems_debug_enable
 *
 *  DESCRIPTION:
 *
 *  This routine enables the specified types of debug checks.
 */

void rtems_debug_enable (
  rtems_debug_control  to_be_enabled
);

/*
 *  rtems_debug_disable
 *
 *  DESCRIPTION:
 *
 *  This routine disables the specified types of debug checks.
 */
 
void rtems_debug_disable (
  rtems_debug_control  to_be_disabled
);
 
/*
 *
 *  _Debug_Is_enabled
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the requested debug level is
 *  enabled, and FALSE otherwise.
 */

boolean _Debug_Is_enabled(
  rtems_debug_control  level
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
