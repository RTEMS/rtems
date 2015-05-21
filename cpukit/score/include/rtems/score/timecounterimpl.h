/**
 * @file
 *
 * @ingroup ScoreTimecounter
 *
 * @brief Timecounter Implementation
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

#ifndef _RTEMS_SCORE_TIMECOUNTERIMPL_H
#define _RTEMS_SCORE_TIMECOUNTERIMPL_H

#include <rtems/score/timecounter.h>
#include <sys/timetc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreTimecounter
 *
 * @{
 */

void _Timecounter_Initialize( void );

void _Timecounter_Set_clock( const struct timespec *ts );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_TIMECOUNTERIMPL_H */
