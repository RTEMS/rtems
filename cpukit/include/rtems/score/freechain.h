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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_FREECHAIN_H
#define _RTEMS_SCORE_FREECHAIN_H

#include <rtems/score/basedefs.h>
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

/**
 * @brief Allocator function.
 */
typedef void *( *Freechain_Allocator )( size_t size );

/**
 * @brief The freechain control.
 */
typedef struct {
  /**
   * @brief Chain of free nodes.
   */
  Chain_Control Free;
} Freechain_Control;

/**
 * @brief Initializes a freechain.
 *
 * This routine initializes the freechain control structure to manage a chain
 * of nodes.  In case the freechain is empty the extend handler is called to
 * get more nodes.
 *
 * @param[in] freechain The freechain control to initialize.
 * @param[in] allocator The allocator function.
 * @param[in] number_nodes The initial number of nodes.
 * @param[in] node_size The node size.
 */
void _Freechain_Initialize(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes,
  size_t               node_size
);

/**
 * @brief Gets a node from the freechain.
 *
 * @param[in] freechain The freechain control.
 * @param[in] allocator The allocator function.
 * @param[in] number_nodes_to_extend The number of nodes in case an extend is
 *   necessary due to an empty freechain.
 * @param[in] node_size The node size.
 *
 * @retval NULL The freechain is empty and the extend operation failed.
 * @retval otherwise Pointer to a node.  The node ownership passes to the
 * caller.
 */
void *_Freechain_Get(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
);

/**
 * @brief Puts a node back onto the freechain.
 *
 * @param[in] freechain The freechain control.
 * @param[in] node The node to put back.  The node may be @c NULL, in this case
 *   the function does nothing.
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
