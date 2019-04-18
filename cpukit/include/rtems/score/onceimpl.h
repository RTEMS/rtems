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

/**
 * @brief Reacts according to the state of once_state.
 *
 * If once_state is ONCE_STATE_INIT, then the once state is set first to
 * ONCE_STATE_RUNNING and after calling the init routine it is set to
 * ONCE_STATE_COMPLETED. If once_state is ONCE_STATE_COMPLETED, nothing
 * happens. If once_state is not either one, this method waits for the
 * Once_Information_Mutex.
 *
 * @param once_state The once state.
 * @param init_routine The initialization routine called if @a once_state is ONCE_STATE_INIT.
 *
 * @return This method always returns zero upon termination.
 */
int _Once( unsigned char *once_state, void ( *init_routine )( void ) );

/**
 * @brief Locks the Once_Information_Mutex and returns the thread life state.
 *
 * @return The thread life state (THREAD_LIFE_PROTECTED).
 */
Thread_Life_state _Once_Lock( void );

/**
 * @brief Unlocks the Once_Information_Mutex and sets the thread life
 *      protection to thread_life_state.
 *
 * @param thread_life_state The thread life state to set the thread life
 *      protection to.
 */
void _Once_Unlock( Thread_Life_state thread_life_state );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_ONCE_H */
