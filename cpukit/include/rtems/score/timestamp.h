/**
 * @file
 *
 * @ingroup SuperCoreTimeStamp
 *
 * @brief Helpers for Manipulating Timestamps
 *
 * This include file contains helpers for manipulating timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESTAMP_H
#define _RTEMS_SCORE_TIMESTAMP_H

/**
 * @defgroup SuperCoreTimeStamp Score Timestamp
 *
 * @ingroup RTEMSScore
 *
 * @brief Score Timestamp
 *
 * This handler encapsulates functionality related to manipulating
 * SuperCore Timestamps.  SuperCore Timestamps may be used to
 * represent time of day, uptime, or intervals.
 *
 * The key attribute of the SuperCore Timestamp handler is that it
 * is a completely opaque handler.  There can be multiple implementations
 * of the required functionality and with a recompile, RTEMS can use
 * any implementation.  It is intended to be a simple wrapper.
 *
 * This handler can be implemented as either struct timespec or
 * unsigned64 bit numbers.  The use of a wrapper class allows the
 * the implementation of timestamps to change on a per architecture
 * basis.  This is an important option as the performance of this
 * handler is critical.
 *
 * @{
 */

#include <rtems/score/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *   Define the Timestamp control type.
 */
typedef int64_t Timestamp_Control;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
