/**
 *  @file  rtems/score/isrlevel.h
 *
 *  This include file defines the ISR Level type.  It exists to
 *  simplify include dependencies.  It is part of the ISR Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ISR_LEVEL_h
#define _RTEMS_SCORE_ISR_LEVEL_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreISR ISR Handler
 */
/**@{*/

/**
 *  The following type defines the control block used to manage
 *  the interrupt level portion of the status register.
 */
typedef uint32_t   ISR_Level;

/**@}*/

#ifdef __cplusplus
}
#endif
#endif

