/*  sysstates.h
 *
 *  This include file contains information regarding the system state.
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

#ifndef __RTEMS_SYSTEM_STATE_h
#define __RTEMS_SYSTEM_STATE_h

#ifdef __cplusplus
extern "C" {
#endif

/* types */

/* enumerated constants */

/*
 *  The following type defines the possible system states.
 */

typedef enum {
  SYSTEM_STATE_BEFORE_INITIALIZATION,   /* start -> end of 1st init part */
  SYSTEM_STATE_BEFORE_MULTITASKING,     /* end of 1st -> beginning of 2nd */
  SYSTEM_STATE_BEGIN_MULTITASKING,      /* just before multitasking starts */
  SYSTEM_STATE_UP,                      /* normal operation */
  SYSTEM_STATE_FAILED                   /* fatal error occurred */
} System_state_Codes;

#define SYSTEM_STATE_CODES_FIRST SYSTEM_STATE_BEFORE_INITIALIZATION
#define SYSTEM_STATE_CODES_LAST  SYSTEM_STATE_FAILED

/*
 *  The following variable indicates whether or not this is 
 *  an multiprocessing system.
 */
 
SCORE_EXTERN boolean _System_state_Is_multiprocessing;

/*
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

#endif
/* end of include file */
