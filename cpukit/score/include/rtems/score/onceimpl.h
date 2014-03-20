/**
 * @file
 *
 * @ingroup ScoreOnce
 *
 * @brief Once API
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_ONCE_H
#define _RTEMS_ONCE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreOnce Once Functions.
 *
 * @ingroup Score
 *
 * @brief The _Once() function for pthread_once() and rtems_gxx_once().
 *
 * @{
 */

int _Once( int *once_state, void (*init_routine)(void) );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_ONCE_H */
