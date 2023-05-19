/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreOnce
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreOnce.
 */

/*
 * Copyright (C) 2014, 2019 embedded brains GmbH & Co. KG
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
