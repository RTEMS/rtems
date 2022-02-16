/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define
 *   ::_User_extensions_Initial_count, ::_User_extensions_Initial_extensions,
 *   and ::_User_extensions_Initial_switch_controls.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_USEREXTDATA_H
#define _RTEMS_SCORE_USEREXTDATA_H

#include <rtems/score/userext.h>
#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreUserExt
 *
 * @{
 */

/**
 * @brief Manages the switch callouts.
 *
 * They are managed separately from other extensions for performance reasons.
 */
typedef struct {
  Chain_Node                              Node;
  User_extensions_thread_switch_extension thread_switch;
}   User_extensions_Switch_control;

/**
 * @brief Manages each user extension set.
 *
 * The switch control is part of the extensions control even if not used due to
 * the extension not having a switch handler.
 */
typedef struct {
  Chain_Node                     Node;
  User_extensions_Switch_control Switch;
  User_extensions_Table          Callouts;
}   User_extensions_Control;

/**
 * @brief The count of initial user extensions.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const size_t _User_extensions_Initial_count;

/**
 * @brief The table of initial user extensions.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern const User_extensions_Table _User_extensions_Initial_extensions[];

/**
 * @brief A spare switch control for each initial user extension.
 *
 * Application provided via <rtems/confdefs.h>.
 */
extern User_extensions_Switch_control
  _User_extensions_Initial_switch_controls[];

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
