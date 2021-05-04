/**
 * @file
 *
 * @ingroup RTEMSScoreOnce
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreOnce.
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
 * @defgroup RTEMSScoreOnce Execute Once Support
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to execute a routine only
 *   once.
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

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_ONCE_H */
