/**
 * @file
 *
 * @ingroup RTEMSScoreFreechain
 *
 * @brief Freechain Handler API
 */
/*
 * Copyright (c) 2013 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_FREECHAIN_H
#define _RTEMS_SCORE_FREECHAIN_H

#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreFreechain Freechain Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief The Freechain Handler.
 *
 * The Freechain Handler is used to manage a chain of nodes, of which size can
 * automatically increase when there is no free node left. This handler
 * provides one data structure: Freechain_Control.
 *
 * @{
 */

/**
 * @brief The freechain control.
 */
typedef struct {
  /**
   * @brief Chain of free nodes.
   */
  Chain_Control Free;
} Freechain_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
