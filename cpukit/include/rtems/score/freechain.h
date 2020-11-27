/**
 * @file
 *
 * @ingroup RTEMSScoreFreechain
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreFreechain which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
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
 * @brief This group contains the Freechain Handler implementation.
 *
 * This handler encapsulates functionality related to the management of
 * free nodes of a user-defined size.  If the chain of free nodes is empty
 * during an allocation request, it can be dynamically extended.
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
