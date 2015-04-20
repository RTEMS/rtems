/**
 * @file
 *
 * @ingroup BSD
 *
 * @brief BSD Compatibility API
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_BSD_H
#define _RTEMS_BSD_H

#include <rtems/score/timecounter.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup BSD BSD Compatibility Support
 *
 * @{
 */

/**
 * @copydoc _Timecounter_Bintime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_bintime( struct bintime *bt )
{
  _Timecounter_Bintime( bt );
}

/**
 * @copydoc _Timecounter_Nanotime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_nanotime( struct timespec *ts )
{
  _Timecounter_Nanotime( ts );
}

/**
 * @copydoc _Timecounter_Microtime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_microtime( struct timeval *tv )
{
  _Timecounter_Microtime( tv );
}

/**
 * @copydoc _Timecounter_Binuptime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_binuptime( struct bintime *bt )
{
  _Timecounter_Binuptime( bt );
}

/**
 * @copydoc _Timecounter_Nanouptime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_nanouptime( struct timespec *ts )
{
  _Timecounter_Nanouptime( ts );
}

/**
 * @copydoc _Timecounter_Microtime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_microuptime( struct timeval *tv )
{
  _Timecounter_Microuptime( tv );
}

/**
 * @copydoc _Timecounter_Getbintime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getbintime( struct bintime *bt )
{
  _Timecounter_Getbintime( bt );
}

/**
 * @copydoc _Timecounter_Getnanotime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getnanotime( struct timespec *ts )
{
  _Timecounter_Getnanotime( ts );
}

/**
 * @copydoc _Timecounter_Getmicrotime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getmicrotime( struct timeval *tv )
{
  _Timecounter_Getmicrotime( tv );
}

/**
 * @copydoc _Timecounter_Getbinuptime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getbinuptime( struct bintime *bt )
{
  _Timecounter_Getbinuptime( bt );
}

/**
 * @copydoc _Timecounter_Getnanouptime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getnanouptime( struct timespec *ts )
{
  _Timecounter_Getnanouptime( ts );
}

/**
 * @copydoc _Timecounter_Getmicrouptime()
 */
RTEMS_INLINE_ROUTINE void rtems_bsd_getmicrouptime( struct timeval *tv )
{
  _Timecounter_Getmicrouptime( tv );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_H */
