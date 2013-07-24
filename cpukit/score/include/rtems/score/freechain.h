/**
 * @file
 *
 * @ingroup ScoreFreechain
 *
 * @brief Freechain Handler API
 */
/*
 * Copyright (c) 2013 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_FREECHAIN_H
#define _RTEMS_SCORE_FREECHAIN_H

#include <stdbool.h>

#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreFreechain Freechain Handler
 *
 * @ingroup Score
 *
 * The Freechain Handler is used to manage a chain of nodes, of which size can
 * automatically increase when there is no free node left. This handler
 * provides one data structure: Freechain_Control.
 *
 * @{
 */

typedef struct Freechain_Control Freechain_Control;

/**
 * @brief Extends the freechain.
 *
 * @param[in] freechain The freechain control.
 *
 * @retval true The freechain contains now at least one node.
 * @retval false Otherwise.
 */
typedef bool ( *Freechain_Extend )( Freechain_Control *freechain );

/**
 * @typedef Freechain_Control
 *
 * This is used to manage freechain's nodes.
 */
struct Freechain_Control {
  Chain_Control     Freechain;
  Freechain_Extend  extend;
};

/**
 * @brief Initializes a freechain.
 *
 * This routine initializes the freechain control structure to manage a chain
 * of nodes.  In case the freechain is empty the extend handler is called to
 * get more nodes.
 *
 * @param[in,out] freechain The freechain control to initialize.
 * @param[in] extend The extend handler.  It is called by _Freechain_Get() in
 * case the freechain is empty.
 */
void _Freechain_Initialize(
  Freechain_Control *freechain,
  Freechain_Extend   extend
);

/**
 * @brief Gets a node from the freechain.
 *
 * @param[in,out] freechain The freechain control.
 *
 * @retval NULL The freechain is empty and the extend operation failed.
 * @retval otherwise Pointer to a node.  The node ownership passes to the
 * caller.
 */
void *_Freechain_Get(
  Freechain_Control *freechain
);

/**
 * @brief Puts a node back onto the freechain.
 *
 * @param[in,out] freechain The freechain control.
 * @param[in] node The node to put back.
 */
void _Freechain_Put(
  Freechain_Control *freechain,
  void              *node
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
