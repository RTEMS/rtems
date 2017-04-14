/**
 * @file rtems/inttypes.h
 *
 * @brief Provide printf() PRIxxx Constante Beyond Standards
 *
 * This include file defines PRIxxx constants beyond those in
 * the C and POSIX standards. These are used to write portable
 * printf() format strings for POSIX and RTEMS types not in
 * <inttypes.h>
 */

/*
 *  COPYRIGHT (c) 2017 On-Line Applications Research Corporation.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_INTTYPES_H
#define _RTEMS_INTTYPES_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS inttypes.h Extensions
 *
 * This module defines portable PRIxxx constants beyond those
 * in the C and POSIX standard.
 */

/** Helper macro to print "modet" in octal */
#if SIZEOF_MODE_T == 8
#define PRIomode_t PRIo64
#elif SIZEOF_MODE_T == 4
#define PRIomode_t PRIo32
#else
#error "PRIomode_t: unsupport size of mode_t"
#endif

/** Helper macro to print "off_t" in octal */
#if SIZEOF_OFF_T == 8
#define PRIooff_t PRIo64
#elif SIZEOF_OFF_T == 4
#define PRIooff_t PRIo32
#else
#error "PRIooff_t: unsupported size of off_t"
#endif

/** Helper macro to print "off_t" in decimal */
#if SIZEOF_OFF_T == 8
#define PRIdoff_t PRId64
#elif SIZEOF_OFF_T == 4
#define PRIdoff_t PRId32
#else
#error "PRIdoff_t: unsupported size of off_t"
#endif

/** Helper macro to print "time_t" in decimal */
#if SIZEOF_TIME_T == 8
#define PRIdtime_t PRId64
#elif SIZEOF_TIME_T == 4
#define PRIdtime_t PRId32
#else
#error "PRIdtime_t: unsupported size of time_t"
#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
