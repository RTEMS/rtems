/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIBSD
 *
 * @brief BSD Compatibility API
 */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_BSD_H
#define _RTEMS_BSD_H

#include <rtems/score/timecounter.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSAPIBSD BSD Compatibility Support
 *
 * @ingroup RTEMSAPI
 *
 * @{
 */

/**
 * @copydoc _Timecounter_Bintime()
 */
static inline void rtems_bsd_bintime( struct bintime *bt )
{
  _Timecounter_Bintime( bt );
}

/**
 * @copydoc _Timecounter_Nanotime()
 */
static inline void rtems_bsd_nanotime( struct timespec *ts )
{
  _Timecounter_Nanotime( ts );
}

/**
 * @copydoc _Timecounter_Microtime()
 */
static inline void rtems_bsd_microtime( struct timeval *tv )
{
  _Timecounter_Microtime( tv );
}

/**
 * @copydoc _Timecounter_Binuptime()
 */
static inline void rtems_bsd_binuptime( struct bintime *bt )
{
  _Timecounter_Binuptime( bt );
}

/**
 * @copydoc _Timecounter_Nanouptime()
 */
static inline void rtems_bsd_nanouptime( struct timespec *ts )
{
  _Timecounter_Nanouptime( ts );
}

/**
 * @copydoc _Timecounter_Microtime()
 */
static inline void rtems_bsd_microuptime( struct timeval *tv )
{
  _Timecounter_Microuptime( tv );
}

/**
 * @copydoc _Timecounter_Getbintime()
 */
static inline void rtems_bsd_getbintime( struct bintime *bt )
{
  _Timecounter_Getbintime( bt );
}

/**
 * @copydoc _Timecounter_Getnanotime()
 */
static inline void rtems_bsd_getnanotime( struct timespec *ts )
{
  _Timecounter_Getnanotime( ts );
}

/**
 * @copydoc _Timecounter_Getmicrotime()
 */
static inline void rtems_bsd_getmicrotime( struct timeval *tv )
{
  _Timecounter_Getmicrotime( tv );
}

/**
 * @copydoc _Timecounter_Getbinuptime()
 */
static inline void rtems_bsd_getbinuptime( struct bintime *bt )
{
  _Timecounter_Getbinuptime( bt );
}

/**
 * @copydoc _Timecounter_Getnanouptime()
 */
static inline void rtems_bsd_getnanouptime( struct timespec *ts )
{
  _Timecounter_Getnanouptime( ts );
}

/**
 * @copydoc _Timecounter_Getmicrouptime()
 */
static inline void rtems_bsd_getmicrouptime( struct timeval *tv )
{
  _Timecounter_Getmicrouptime( tv );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_H */
