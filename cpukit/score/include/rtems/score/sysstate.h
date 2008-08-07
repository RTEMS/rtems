/** 
 *  @file  rtems/score/sysstate.h
 *
 *  This include file contains information regarding the system state.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SYSSTATE_H
#define _RTEMS_SCORE_SYSSTATE_H

/**
 *  @defgroup ScoreSysState System State Handler
 *
 *  This handler encapsulates functionality related to the management of the
 *  internal system state of RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/* types */

/* enumerated constants */

/**
 *  The following type defines the possible system states.
 */
typedef enum {
  /** This indicates that the system state is between the start
   *  of rtems_initialize_executive_early and the end of the first
   *  phase of initialization.
   */
  SYSTEM_STATE_BEFORE_INITIALIZATION,
  /** This indicates that the system state is between end of the first
   *  phase of initializatin but before  multitasking is started.
   */
  SYSTEM_STATE_BEFORE_MULTITASKING,
  /** This indicates that the system state is attempting to initiate 
   *  multitasking. 
   */
  SYSTEM_STATE_BEGIN_MULTITASKING,
  /** This indicates that the system is up and operating normally. */
  SYSTEM_STATE_UP,
  /** This indicates that the system is in the midst of a shutdown. */
  SYSTEM_STATE_SHUTDOWN,
  /** This indicates that a fatal error has occurred. */
  SYSTEM_STATE_FAILED
} System_state_Codes;

/** This defines the first system state. */
#define SYSTEM_STATE_CODES_FIRST SYSTEM_STATE_BEFORE_INITIALIZATION
/** This defines the highest value system state. */
#define SYSTEM_STATE_CODES_LAST  SYSTEM_STATE_FAILED

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  The following variable indicates whether or not this is
 *  an multiprocessing system.
 */
SCORE_EXTERN boolean _System_state_Is_multiprocessing;
#endif

/**
 *  The following variable contains the current system state.
 */
SCORE_EXTERN System_state_Codes _System_state_Current;

/*
 *  Make it possible for the application to get the system state information.
 */

#include <rtems/score/sysstate.inl>

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
