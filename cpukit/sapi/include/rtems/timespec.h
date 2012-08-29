/**
 *  @file  rtems/sapi/timespec.h
 *
 *  @ingroup 
 *
 *  @brief Timespec API
 *
 *  This include file contains API for manipulating timespecs.
 */

/*
 *  Copyright (c) 2012-.
 *  Krzysztof Miesowicz krzysztof.miesowicz@gmail.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TIMESPEC_H
#define _RTEMS_TIMESPEC_H

#include <rtems/score/timespec.h>

/**
 *  @defgroup TimespecAPI Timespec
 *
 *  @ingroup ClassicRTEMS
 *
 *  @brief Timespec API
 *
 */
/**@{*/

#include <stdbool.h> /* bool */
#include <stdint.h> /* uint32_t */
#include <time.h> /* struct timespec */

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/timespec.inl>

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

