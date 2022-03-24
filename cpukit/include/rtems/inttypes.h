/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_INTTYPES_H
#define _RTEMS_INTTYPES_H

#include <inttypes.h>
#include <rtems/score/cpuopts.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS inttypes.h Extensions
 *
 * @ingroup RTEMSAPIPrintSupport
 *
 * This module defines portable PRIxxx constants beyond those
 * in the C and POSIX standard.
 */

/** Helper macro to print "modet" in octal */
#if __RTEMS_SIZEOF_MODE_T__ == 8
#define PRIomode_t PRIo64
#elif __RTEMS_SIZEOF_MODE_T__ == 4
#define PRIomode_t PRIo32
#else
#error "PRIomode_t: unsupported size of mode_t"
#endif

/** Helper macro to print "off_t" in octal */
#if __RTEMS_SIZEOF_OFF_T__ == 8
#define PRIooff_t PRIo64
#elif __RTEMS_SIZEOF_OFF_T__ == 4
#define PRIooff_t PRIo32
#else
#error "PRIooff_t: unsupported size of off_t"
#endif

/** Helper macro to print "off_t" in decimal */
#if __RTEMS_SIZEOF_OFF_T__ == 8
#define PRIdoff_t PRId64
#elif __RTEMS_SIZEOF_OFF_T__ == 4
#define PRIdoff_t PRId32
#else
#error "PRIdoff_t: unsupported size of off_t"
#endif

/** Helper macro to print "time_t" in decimal */
#if __RTEMS_SIZEOF_TIME_T__ == 8
#define PRIdtime_t PRId64
#elif __RTEMS_SIZEOF_TIME_T__ == 4
#define PRIdtime_t PRId32
#else
#error "PRIdtime_t: unsupported size of time_t"
#endif

/** Helper macro to print "blksize_t" in hexadecimal */
#if __RTEMS_SIZEOF_BLKSIZE_T__ == 8
#define PRIxblksize_t PRIx64
#elif __RTEMS_SIZEOF_BLKSIZE_T__ == 4
#define PRIxblksize_t PRIx32
#else
/* Warn and fall back to "long" */
#warning "unsupported size of blksize_t"
#define PRIxblksize_t "lx"
#endif

/** Helper macro to print "blkcnt_t" in hexadecimal */
#if __RTEMS_SIZEOF_BLKCNT_T__ == 8
#define PRIxblkcnt_t PRIx64
#elif __RTEMS_SIZEOF_BLKCNT_T__ == 4
#define PRIxblkcnt_t PRIx32
#else
/* Warn and fall back to "long" */
#warning "unsupported size of blkcnt_t"
#define PRIxblkcnt_t "lx"
#endif

/*
 * Various inttypes.h-stype macros to assist printing
 * certain system types on different targets.
 */

#define PRIxrtems_id PRIx32

/* c.f. cpukit/score/include/rtems/score/priority.h */
#define PRIdPriority_Control PRIu64
#define PRIxPriority_Control PRIx64
/* rtems_task_priority is a typedef to Priority_Control */
#define PRIdrtems_task_priority PRIu32
#define PRIxrtems_task_priority PRIx32

/* c.f. cpukit/score/include/rtems/score/watchdog.h */
#define PRIdWatchdog_Interval PRIu32
/* rtems_interval is a typedef to Watchdog_Interval */
#define PRIdrtems_interval    PRIdWatchdog_Interval

/* c.f. cpukit/score/include/rtems/score/thread.h */
#define PRIdThread_Entry_numeric_type PRIuPTR
/* rtems_task_argument is a typedef to Thread_Entry_numeric_type */
#define PRIdrtems_task_argument PRIdThread_Entry_numeric_type

/* rtems_event_set is a typedef to uint32_t */
#define PRIxrtems_event_set PRIx32

/* newlib defines pthread_t as a typedef to __uint32_t which matches
 * RTEMS expectations for an Object ID.
 */
#define PRIxpthread_t PRIx32

/* rtems_signal_set is a typedef to uint32_t */
#define PRIxrtems_signal_set PRIx32

/* newlib's ino_t is a typedef to __uint64_t */
#define PRIuino_t PRIu64

/* newlib's ino_t is a typedef to __uint64_t */
#define PRIxino_t PRIx64

/* ioctl_command_t */
#define PRIdioctl_command_t "ld"

/* rtems_blkdev_bnum */
#define PRIdrtems_blkdev_bnum PRId32

/* rtems_blkdev_bnum */
#define PRIdrtems_blkdev_bnum PRId32

/* rtems_vector_number */
#define PRIdrtems_vector_number PRId32

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
