/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief User Extension Handler Data Structures
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
