/**
 * @file
 *
 * @ingroup ScoreTOD
 *
 * @brief Time of Day Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TOD_H
#define _RTEMS_SCORE_TOD_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the nanoseconds since the last clock tick.
 *
 * @ingroup ScoreTOD
 *
 * @return The nanoseconds since the last clock tick.
 */
typedef uint32_t ( *TOD_Nanoseconds_since_last_tick_routine )( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
