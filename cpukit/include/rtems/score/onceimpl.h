/**
 * @file
 *
 * @ingroup RTEMSScoreOnce
 *
 * @brief Once API
 */

/*
 * Copyright (c) 2014, 2019 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreOnce Once Functions.
 *
 * @ingroup RTEMSScore
 *
 * @brief The _Once() function for pthread_once() and rtems_gxx_once().
 *
 * @{
 */

int _Once( unsigned char *once_state, void ( *init_routine )( void ) );

Thread_Life_state _Once_Lock( void );

void _Once_Unlock( Thread_Life_state thread_life_state );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_ONCE_H */
